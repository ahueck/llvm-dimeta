
//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
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
#include "ValuePath.h"
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
#include <type_traits>
#include <unordered_map>

namespace dimeta::type {

namespace reset {

using GepToDIMemberMap = std::unordered_map<const llvm::GEPOperator*, llvm::DIDerivedType*>;

namespace detail {

template <typename T, typename InstTy>
std::optional<const T*> get_operand_to(const InstTy* memory_instruction) {
  static_assert(std::is_same_v<InstTy, llvm::StoreInst> || std::is_same_v<InstTy, llvm::LoadInst>,
                "Expected load or store instruction");
  const auto* memory_target = memory_instruction->getPointerOperand();
  if (llvm::isa<T>(memory_target)) {
    return llvm::dyn_cast<T>(memory_target);
  }

  if (auto bcast = llvm::dyn_cast<llvm::BitCastInst>(memory_target)) {
    if (llvm::isa<T>(bcast->getOperand(0))) {
      return llvm::dyn_cast<T>(bcast->getOperand(0));
    }
  }

  return {};
}

}  // namespace detail

template <typename T>
bool store_to(const llvm::StoreInst* store) {
  return detail::get_operand_to<T>(store).has_value();
}

template <typename T>
bool load_to(const llvm::LoadInst* store) {
  return detail::get_operand_to<T>(store).has_value();
}

std::optional<llvm::DIType*> reset_load_related_basic(const dataflow::ValuePath& path, llvm::DIType* type_to_reset,
                                                      const llvm::LoadInst* load) {
  auto type = type_to_reset;

  if (load_to<llvm::GlobalVariable>(load) || load_to<llvm::AllocaInst>(load)) {
    LOG_DEBUG("Do not reset DIType based on load to global,alloca")
    return type;
  }

  if (auto* maybe_ptr_to_type = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
    if ((maybe_ptr_to_type->getTag() == llvm::dwarf::DW_TAG_pointer_type ||
         maybe_ptr_to_type->getTag() == llvm::dwarf::DW_TAG_reference_type)) {
      LOG_DEBUG("Load of pointer-like " << log::ditype_str(maybe_ptr_to_type))
    }

    auto base_type = maybe_ptr_to_type->getBaseType();

    if (auto* composite = llvm::dyn_cast<llvm::DICompositeType>(base_type)) {
      LOG_DEBUG("Have ptr to composite " << log::ditype_str(composite))
      auto type_tbaa = tbaa::resolve_tbaa(base_type, *load);
      if (type_tbaa) {
        return type_tbaa;
      }
    }
    return base_type;
  }

  return type;
}

std::optional<llvm::DIType*> reset_store_related_basic(const dataflow::ValuePath&, llvm::DIType* type_to_reset,
                                                       const llvm::StoreInst* store_inst) {
  auto type = type_to_reset;

  if (store_to<llvm::GlobalVariable>(store_inst) || store_to<llvm::AllocaInst>(store_inst)) {
    // Relevant in "heap_lulesh_mock_char.cpp"
    LOG_DEBUG("Store to alloca/global, return " << log::ditype_str(type))
    return type;
  }

  if (!llvm::isa<llvm::DIDerivedType>(type)) {
    LOG_DEBUG("Store resolved, return " << log::ditype_str(type))
    return type;
  }

  auto* derived_type = llvm::cast<llvm::DIDerivedType>(type);

  if (derived_type->getTag() == llvm::dwarf::DW_TAG_member) {
    auto* member_base               = derived_type->getBaseType();
    const bool is_array_type_member = member_base->getTag() == llvm::dwarf::DW_TAG_array_type;
    // Need to look at base type for array-type member of struct. Tests w.r.t. gep:
    // 1. array_composite.c
    // 2. array_composite_offset_zero.c
    // 3. array_composite_sub_offset_zero.c
    // 4. global_nested.c
    if (is_array_type_member) {
      return llvm::cast<llvm::DICompositeType>(member_base)->getBaseType();
    }
    return member_base;
  }

  const bool is_pointer = derived_type->getTag() == llvm::dwarf::DW_TAG_pointer_type ||
                          derived_type->getTag() == llvm::dwarf::DW_TAG_reference_type;
  if (is_pointer) {
    if (auto* may_be_ptr_to_ptr = llvm::dyn_cast<llvm::DIDerivedType>(derived_type->getBaseType())) {
      // Pointer to pointer by default remove one level for RHS assignment type w.r.t. store:
      const auto is_ptr_to_ptr = may_be_ptr_to_ptr->getTag() == llvm::dwarf::DW_TAG_pointer_type ||
                                 may_be_ptr_to_ptr->getTag() == llvm::dwarf::DW_TAG_reference_type;
      if (is_ptr_to_ptr) {
        LOG_DEBUG("Store to ptr-ptr, return " << log::ditype_str(may_be_ptr_to_ptr))
        return may_be_ptr_to_ptr;
      }
    }

    if (auto* ptr_to_composite = llvm::dyn_cast<llvm::DICompositeType>(derived_type->getBaseType())) {
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

  LOG_DEBUG("Store resolved, return " << log::ditype_str(type))
  return type;
}

template <typename Iter, typename Iter2>
std::optional<llvm::DIType*> reset_ditype(llvm::DIType* type_to_reset, const dataflow::ValuePath& path,
                                          const Iter& path_iter, const Iter2&) {
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
    return reset::reset_store_related_basic(path, type.value(), store_inst);
  }

  LOG_DEBUG(">> skipping");

  return type;
}

}  // namespace reset

std::optional<llvm::DIType*> find_type(const dataflow::CallValuePath& call_path) {
  auto type = root::find_type_root(call_path);

  if (!type) {
    LOG_DEBUG("find_type_root failed to find a type for path " << call_path.path)
    return {};
  }

  const auto path_end = call_path.path.path_to_value.rend();
  for (auto path_iter = call_path.path.path_to_value.rbegin(); path_iter != path_end; ++path_iter) {
    if (!type) {
      break;
    }
    if (llvm::isa<llvm::GEPOperator>(*path_iter)) {
      auto* gep = llvm::cast<llvm::GEPOperator>(*path_iter);
      LOG_DEBUG("Path iter gep for extraction is currently " << *gep);
      // TODO: Maybe we could somehow get more info on the underlying type from the dataflow path
      //       if this returns an empty result due to forward decls?
      const auto gep_result = gep::extract_gep_dereferenced_type(type.value(), *gep);
      type                  = gep_result.type;
      if (gep_result.member) {
        LOG_DEBUG("Using gep member type result")
        type = gep_result.member;
      }
      LOG_DEBUG("Gep reset type is " << log::ditype_str(type.value_or(nullptr)) << "\n")
      continue;
    }
    LOG_DEBUG("Extracted type w.r.t. gep: " << log::ditype_str(*type));
    type = reset::reset_ditype(type.value(), call_path.path, path_iter, path_end).value_or(type.value());
    LOG_DEBUG("reset_ditype result " << log::ditype_str(type.value_or(nullptr)) << "\n")
  }

  if (type) {
    // If last node is a store inst, try to extract type via TBAA
    const auto* const start_node = llvm::dyn_cast_or_null<llvm::StoreInst>(*call_path.path.start_value());
    if (start_node) {
      auto type_tbaa = tbaa::resolve_tbaa(type.value(), *llvm::dyn_cast<llvm::Instruction>(start_node));
      if (type_tbaa) {
        type = type_tbaa.value();
      }
    }
  }

  return type;
}
}  // namespace dimeta::type
