
//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIFinder.h"
#include "DIRootType.h"
#include "DataflowAnalysis.h"
#include "DefUseAnalysis.h"
#include "GEP.h"
#include "TBAA.h"
#include "Util.h"
#include "support/Logger.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/ilist_iterator.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <iterator>

namespace dimeta::type {

namespace reset {

using GepToDIMemberMap = std::unordered_map<const llvm::GEPOperator*, llvm::DIDerivedType*>;

template <typename T>
bool load_to(const llvm::LoadInst* load) {
  auto load_target = load->getPointerOperand();
  if (llvm::isa<T>(load_target)) {
    return true;
  }
  if (auto bcast = llvm::dyn_cast<llvm::BitCastInst>(load_target)) {
    if (llvm::isa<T>(bcast->getOperand(0))) {
      return true;
    }
  }
  return false;
}

template <typename T>
std::optional<const T*> get_store_to(const llvm::StoreInst* store) {
  const auto* store_target = store->getPointerOperand();
  if (llvm::isa<T>(store_target)) {
    return llvm::dyn_cast<T>(store_target);
  }
  if (auto bcast = llvm::dyn_cast<llvm::BitCastInst>(store_target)) {
    if (llvm::isa<T>(bcast->getOperand(0))) {
      return llvm::dyn_cast<T>(bcast->getOperand(0));
    }
  }
  if (auto bcast = llvm::dyn_cast<llvm::ConstantExpr>(store_target)) {
    if (llvm::isa<T>(bcast->getOperand(0))) {
      return llvm::dyn_cast<T>(bcast->getOperand(0));
    }
  }
  return {};
}

template <typename T>
bool store_to(const llvm::StoreInst* store) {
  return get_store_to<T>(store).has_value();
}

std::optional<llvm::DIType*> reset_load_related_basic(const dataflow::ValuePath& path, llvm::DIType* type_to_reset,
                                                      const llvm::LoadInst* load) {
  auto type = type_to_reset;

  if (load_to<llvm::GlobalVariable>(load) || load_to<llvm::AllocaInst>(load)) {
    LOG_DEBUG("Do not reset DIType based on load to global,alloca")
    return type;
  }

  if (auto* ptr_to_type = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
    auto base_type = ptr_to_type->getBaseType();
    if (auto* composite = llvm::dyn_cast<llvm::DICompositeType>(base_type)) {
      LOG_DEBUG("Have ptr to composite " << log::ditype_str(composite))
      auto type_tbaa = tbaa::resolve_tbaa(base_type, *load);
      if(type_tbaa){
        return type_tbaa;
      }
    }
    return base_type;
  }

  return type;
}

std::optional<llvm::DIType*> reset_store_related_basic(const dataflow::ValuePath& path, llvm::DIType* type_to_reset,
                                                       const llvm::StoreInst* store_inst,
                                                       const GepToDIMemberMap& gep2member) {
  auto type = type_to_reset;

  if (auto* array_to_composite = llvm::dyn_cast<llvm::DICompositeType>(type)) {
    if (array_to_composite->getTag() == llvm::dwarf::DW_TAG_array_type) {
      LOG_DEBUG("Loaded from extracted type of array type " << log::ditype_str(array_to_composite->getBaseType()))
      return array_to_composite->getBaseType();
    }
  }

  if (store_to<llvm::GlobalVariable>(store_inst) || store_to<llvm::AllocaInst>(store_inst)) {
    // Relevant in "heap_lulesh_mock_char.cpp"
    LOG_DEBUG("Store to alloca/global, return " << log::ditype_str(type))
    return type;
  }

  if (auto* ptr_type = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
    // LOG_DEBUG(*ptr_type)
    if (auto* ptr_to_ptr = llvm::dyn_cast<llvm::DIDerivedType>(ptr_type->getBaseType())) {
      // Pointer to pointer by default remove one level for RHS assignment type w.r.t. store:
      const auto is_ptr_to_ptr = ptr_to_ptr->getTag() == llvm::dwarf::DW_TAG_pointer_type;

      auto gep = get_store_to<llvm::GetElementPtrInst>(store_inst);
      if (is_ptr_to_ptr && gep) {
        if (auto gep_op = llvm::dyn_cast<llvm::GEPOperator>(gep.value())) {
          if (auto member = gep2member.find(gep_op); member != std::end(gep2member)) {
            //            auto member_di = member->second;
            LOG_DEBUG("Gep returns member to struct, return ptr_type " << log::ditype_str(ptr_type))
            return ptr_type;
          }
          LOG_DEBUG("Gep operator does not return member " << *gep_op)
        }
      }

      if (is_ptr_to_ptr) {
        LOG_DEBUG("Store to ptr-ptr, return " << log::ditype_str(ptr_to_ptr))
        return ptr_to_ptr;
      }
    }
    if (auto* ptr_to_composite = llvm::dyn_cast<llvm::DICompositeType>(ptr_type->getBaseType())) {
      if (store_to<llvm::LoadInst>(store_inst)) {
        // Triggers for "heap_lhs_obj_opt.c" (llvm 14/15)
        auto composite_members = ptr_to_composite->getElements();
        assert(!composite_members.empty() && "Store to composite assumed to be store to first member!");
        auto store_di_target = llvm::dyn_cast<llvm::DIDerivedType>(composite_members[0])->getBaseType();
        LOG_DEBUG("Store to a 'load of a composite type', assume first member as target "
                  << log::ditype_str(store_di_target))
        return store_di_target;
      }
    }
  }

  if (auto store_to_composite = llvm::dyn_cast<llvm::DICompositeType>(type)) {
    LOG_DEBUG("Store to compound, return it " << log::ditype_str(type))
    return type;
  }

  LOG_DEBUG("Store resolved, return " << log::ditype_str(type))
  return type;
}

template <typename Iter, typename Iter2>
std::optional<llvm::DIType*> reset_ditype(llvm::DIType* type_to_reset, const dataflow::ValuePath& path,
                                          const Iter& path_iter, const Iter2& path_end,
                                          const reset::GepToDIMemberMap& gep2member) {
  std::optional<llvm::DIType*> type = type_to_reset;
  if (!type) {
    LOG_DEBUG("No type to reset!")
    return {};
  }

  auto next_value = path_iter;
  LOG_DEBUG("Type to reset: " << log::ditype_str(*type));
  LOG_DEBUG(">> based on IR: " << **next_value);

  if (const auto* load = llvm::dyn_cast<llvm::LoadInst>(*next_value)) {
    // Re-set the DIType from the gep, if presence of:
    // - a load after a gep is likely the first element of the composite type
    // - a load also resolves to the basetype w.r.t. an array composite
    LOG_DEBUG("Reset based on load")
    return reset::reset_load_related_basic(path, type.value(), load);
  }

  if (const auto* store_inst = llvm::dyn_cast<llvm::StoreInst>(*next_value)) {
    // - a store with a ditype(array) is likely the first element of the array
    LOG_DEBUG("Reset based on store")
    return reset::reset_store_related_basic(path, type.value(), store_inst, gep2member);
  }

  LOG_DEBUG(">> skipping");

  return type;
}

}  // namespace reset

std::optional<llvm::DIType*> find_type(const dataflow::ValuePath& path) {
  auto type = root::find_type_root(path);

  if (!type) {
    LOG_DEBUG("find_type_root failed to find a type for path " << path)
    return {};
  }

  reset::GepToDIMemberMap gep_to_member_map;

  const auto path_end = path.path_to_value.rend();
  for (auto path_iter = path.path_to_value.rbegin(); path_iter != path_end; ++path_iter) {
    if (!type) {
      break;
    }
    if (llvm::isa<llvm::GEPOperator>(*path_iter)) {
      auto* gep = llvm::cast<llvm::GEPOperator>(*path_iter);
      LOG_DEBUG("Path iter gep for extraction is currently " << *gep);
      const auto gep_result = gep::extract_gep_deref_type(type.value(), *gep);
      type                  = gep_result.type;
      if (gep_result.member) {
        gep_to_member_map.try_emplace(gep, gep_result.member.value());
      }
      LOG_DEBUG("Gep reset type is " << log::ditype_str(type.value_or(nullptr)) << "\n")
      continue;
    }
    LOG_DEBUG("Extracted type w.r.t. gep: " << log::ditype_str(*type));
    type = reset::reset_ditype(type.value(), path, path_iter, path_end, gep_to_member_map).value_or(type.value());
    LOG_DEBUG("reset_ditype result " << log::ditype_str(type.value_or(nullptr)) << "\n")
  }

  if (type) {
    auto type_tbaa = tbaa::resolve_tbaa(type.value(), path);
    if (type_tbaa) {
      return type_tbaa;
    }
  }

  return type;
}
}  // namespace dimeta::type
