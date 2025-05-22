
//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIFinder.h"
#include "DIRootType.h"
#include "DIUtil.h"
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

bool is_array_gep_with_non_const_indices(const llvm::GetElementPtrInst* gep) {
#if LLVM_VERSION_MAJOR > 12
  auto indices = gep->indices();
#else
  auto indices = llvm::make_range(gep->idx_begin(), gep->idx_end());
#endif
  for (const auto& index : indices) {
    if (!llvm::isa<llvm::ConstantInt>(index.get())) {
      return true;
    }
  }
  return false;
}

bool is_array_gep(const llvm::GetElementPtrInst* gep) {
  if (!gep) {
    return false;
  }
  return detail::is_array_gep_with_non_const_indices(gep) || gep->getSourceElementType()->isArrayTy();
}

}  // namespace detail

template <typename T>
bool store_to(const llvm::StoreInst* store) {
  return detail::get_operand_to<T>(store).has_value();
}

template <typename T>
bool load_to(const llvm::LoadInst* load) {
  return detail::get_operand_to<T>(load).has_value();
}

bool load_for_array_gep(const llvm::LoadInst* load) {
  for (const auto* user : load->users()) {
    if (const auto* gep = llvm::dyn_cast<llvm::GetElementPtrInst>(user)) {
      return detail::is_array_gep(gep);
    }
  }
  return false;
}

bool load_of_array_gep(const llvm::LoadInst* load) {
  return detail::is_array_gep(llvm::dyn_cast<llvm::GetElementPtrInst>(load->getPointerOperand()));
}

bool store_to_array_gep(const llvm::StoreInst* store) {
  auto gep = detail::get_operand_to<llvm::GetElementPtrInst>(store);
  if (!gep) {
    return false;
  }
  return detail::is_array_gep_with_non_const_indices(gep.value());
}

std::optional<llvm::DIType*> reset_load_related_basic(const dataflow::ValuePath& path, llvm::DIType* type_to_reset,
                                                      const llvm::LoadInst* load) {
  auto* type = type_to_reset;

  // if (auto* member_composite_type = llvm::dyn_cast<llvm::DICompositeType>(type)) {
  //   auto result = di::util::resolve_byte_offset_to_member_of(member_composite_type, 0);
  //   if (result) {
  //     LOG_DEBUG("Return type of load " << log::ditype_str(result->type_of_member.value_or(nullptr)))
  //     return result->type_of_member;
  //   }
  // }

  if (load_to<llvm::GlobalVariable>(load) || load_to<llvm::AllocaInst>(load)) {
    // if (auto* maybe_ptr_to_type = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
    //   if ((maybe_ptr_to_type->getTag() == llvm::dwarf::DW_TAG_pointer_type ||
    //        maybe_ptr_to_type->getTag() == llvm::dwarf::DW_TAG_reference_type)) {
    //     LOG_DEBUG("Load of pointer-like " << log::ditype_str(maybe_ptr_to_type))
    //     if (load->getPointerOperandType()->isOpaquePointerTy()) {
    //       return maybe_ptr_to_type->getBaseType();
    //     }
    //   }
    // }

    LOG_DEBUG("Do not reset DIType based on load to global,alloca")
    return type;
  }

  if (!di::util::is_member(*type)) {
    auto comp = di::util::desugar(*type);
    LOG_DEBUG("Desugared load to " << log::ditype_str(comp.value_or(nullptr)))
    if (comp && !load_to<llvm::Argument>(load) && !load_of_array_gep(load) && !load_for_array_gep(load)) {
      LOG_DEBUG("Loading first pointer member?")

      auto result = di::util::resolve_byte_offset_to_member_of(comp.value(), 0);
      if (result) {
        LOG_DEBUG("Return type of load " << log::ditype_str(result->type_of_member.value_or(nullptr)))
        return result->type_of_member;
      }
    }
  }

  // if()

  // a (last?) load to a GEP of a composite likely loads the first member in an optimized context:
  const bool last_load = path.start_value().value_or(nullptr) == load;
  if (last_load && load_to<llvm::GetElementPtrInst>(load)) {
    if (auto* may_be_member_type = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
      LOG_DEBUG("Load on GEP, return basetype " << log::ditype_str(may_be_member_type->getBaseType()))
      if (di::util::is_member(*may_be_member_type)) {
        auto type_of_member = may_be_member_type->getBaseType();
        if (auto member = llvm::dyn_cast<llvm::DIDerivedType>(type_of_member)) {
          may_be_member_type = member;
          if (auto* member_composite_type = llvm::dyn_cast<llvm::DICompositeType>(member->getBaseType())) {
            auto members_of_composite_type = di::util::get_composite_members(*member_composite_type);
            assert(members_of_composite_type.size() > 0 && "Load to composite expects at least one member");
            return (*members_of_composite_type.begin());
          }
        }
      }
    }
  }

  if (auto* maybe_ptr_to_type = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
    if (di::util::is_pointer(*maybe_ptr_to_type)) {
      LOG_DEBUG("Load of pointer-like " << log::ditype_str(maybe_ptr_to_type))
    }

    auto* base_type = maybe_ptr_to_type->getBaseType();
    if (di::util::is_array_member(*type)) {
      LOG_DEBUG("Load of array-like " << log::ditype_str(base_type))
      // auto type_de = di::util::desugar(*base_type, 1);
      if (auto underlying_type = llvm::dyn_cast<llvm::DICompositeType>(base_type)->getBaseType()) {
        return underlying_type;
      }
    }

#if DIMETA_USE_TBAA == 1
    if (auto* composite = llvm::dyn_cast<llvm::DICompositeType>(base_type)) {
      LOG_DEBUG("Have ptr to composite " << log::ditype_str(composite))
      auto type_tbaa = tbaa::resolve_tbaa(base_type, *load);
      if (type_tbaa) {
        return type_tbaa;
      }
    }
#endif
    return base_type;
  }

  return type;
}

std::optional<llvm::DIType*> reset_store_related_basic(const dataflow::ValuePath&, llvm::DIType* type_to_reset,
                                                       const llvm::StoreInst* store_inst) {
  auto* type = type_to_reset;

  if (store_to<llvm::GlobalVariable>(store_inst) || store_to<llvm::AllocaInst>(store_inst)) {
    // Relevant in "heap_lulesh_mock_char.cpp"
    LOG_DEBUG("Store to alloca/global, return " << log::ditype_str(type))
    return type;
  }

  if (!di::util::is_array_member(*type)) {
    // !di::util::is_array(*type) &&
    if (di::util::is_array(*type)) {
      return llvm::cast<llvm::DICompositeType>(type)->getBaseType();
    }

    const auto is_non_pointer_member = [&](auto& type_) {
      return di::util::is_non_static_member(type_) &&
             !di::util::is_pointer_like(*llvm::cast<llvm::DIDerivedType>(&type_)->getBaseType());
    };

    auto comp = di::util::desugar(*type);
    LOG_DEBUG("Desugared " << log::ditype_str(comp.value_or(nullptr)))
    if (comp && (store_to<llvm::LoadInst>(store_inst) || store_to<llvm::Argument>(store_inst) ||
                 store_to_array_gep(store_inst) || is_non_pointer_member(*type))) {
      //&& store_to<llvm::LoadInst>(store_inst)  // && !store_to_array_gep(store_inst)) {
      LOG_DEBUG("Storing to first pointer member?")
      auto result = di::util::resolve_byte_offset_to_member_of(comp.value(), 0);
      if (result) {
        LOG_DEBUG("Return type of store " << log::ditype_str(result->type_of_member.value_or(nullptr)))
        return result->type_of_member;
      }
    }
  }

  if (!llvm::isa<llvm::DIDerivedType>(type)) {
    LOG_DEBUG("Store resolved, return " << log::ditype_str(type))
    return type;
  }

  auto* derived_type = llvm::cast<llvm::DIDerivedType>(type);

  if (di::util::is_non_static_member(*derived_type)) {
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

  if (di::util::is_pointer(*derived_type)) {
    if (auto* may_be_ptr_to_ptr = llvm::dyn_cast<llvm::DIDerivedType>(derived_type->getBaseType())) {
      // Pointer to pointer by default remove one level for RHS assignment type w.r.t. store:
      const auto is_ptr_to_ptr = di::util::is_pointer(*may_be_ptr_to_ptr);
      if (is_ptr_to_ptr) {
        LOG_DEBUG("Store to ptr-ptr, return " << log::ditype_str(may_be_ptr_to_ptr))
        return may_be_ptr_to_ptr;
      }
    }

    // if (auto* ptr_to_composite = llvm::dyn_cast<llvm::DICompositeType>(derived_type->getBaseType())) {
    //   if (store_to<llvm::LoadInst>(store_inst)) {
    //     // Triggers for "heap_lhs_obj_opt.c" (llvm 14/15)
    //     auto composite_members = ptr_to_composite->getElements();
    //     assert(!composite_members.empty() && "Store to composite assumed to be store to first member!");
    //     // auto store_di_target = llvm::dyn_cast<llvm::DIDerivedType>(composite_members[0])->getBaseType();

    //     for (auto* member : composite_members) {
    //       if (di::util::is_non_static_member(*member)) {
    //         auto* store_di_target = llvm::dyn_cast<llvm::DIDerivedType>(member);
    //         LOG_DEBUG("Store to a 'load of a composite type', assume first member as target "
    //                   << log::ditype_str(store_di_target))
    //         return store_di_target->getBaseType();
    //       }
    //     }
    //   }
    // }
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

  const auto& next_value = path_iter;
  LOG_DEBUG("Type to reset: " << log::ditype_str(*type));
  LOG_DEBUG(">> based on IR: " << **next_value);

  if (llvm::isa<llvm::GEPOperator>(*next_value)) {
    LOG_DEBUG("Reset based on GEP")
    auto* gep = llvm::cast<llvm::GEPOperator>(*next_value);
    // LOG_DEBUG("Path iter gep for extraction is currently " << *gep);
    // TODO: Maybe we could somehow get more info on the underlying type from the dataflow path
    //       if this returns an empty result due to forward decls?
    const auto gep_result = gep::extract_gep_dereferenced_type(type.value(), *gep);
    if (gep_result.member && !gep_result.use_type) {
      LOG_DEBUG("Using gep member type result")
      return gep_result.member;
    }
    return gep_result.type;
  }

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
    LOG_DEBUG("Extracted type w.r.t. gep: " << log::ditype_str(*type));
    type = reset::reset_ditype(type.value(), call_path.path, path_iter, path_end).value_or(type.value());
    LOG_DEBUG("reset_ditype result " << log::ditype_str(type.value_or(nullptr)) << "\n")
  }

#if DIMETA_USE_TBAA == 1
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
#endif
  return type;
}
}  // namespace dimeta::type
