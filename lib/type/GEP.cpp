//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "GEP.h"

#include "DIVisitorUtil.h"
#include "support/Logger.h"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Use.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <DIVisitor.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>

namespace dimeta::gep {

namespace util {
inline bool is_byte_indexing(const llvm::GEPOperator* gep) {
  return gep->getSourceElementType()->isIntegerTy(8);
}

inline bool is_first_non_zero_indexing(const llvm::GEPOperator* gep) {
  if (auto* const_idx = llvm::dyn_cast<llvm::ConstantInt>((*gep->idx_begin()).get())) {
    const int64_t index = const_idx->getValue().getSExtValue();
    return index > 0;
  }
  return false;
}
}  // namespace util

struct GepIndices {
  const llvm::GEPOperator* gep;
  llvm::SmallVector<uint64_t, 4> indices_;
  bool skipped{false};
  bool is_byte_access{false};
  using Iter = llvm::SmallVector<uint64_t, 4>::const_iterator;

  inline llvm::iterator_range<Iter> indices() const {
    return llvm::iterator_range<Iter>(indices_);
  }

  inline size_t size() const {
    return indices_.size();
  }

  inline bool empty() const {
    return size() == 0;
  }

  inline bool skipped_first() const {
    return skipped;
  }

  inline bool byte_access() const {
    return is_byte_access;
  }

  static GepIndices create(const llvm::GEPOperator* inst, bool skip_first = true);
};

GepIndices GepIndices::create(const llvm::GEPOperator* inst, bool skip_first) {
  GepIndices gep_ind;
  gep_ind.gep            = inst;
  gep_ind.skipped        = skip_first;
  gep_ind.is_byte_access = util::is_byte_indexing(gep_ind.gep);

#if LLVM_VERSION_MAJOR > 12
  for (const auto& index : inst->indices()) {
#else
  for (const auto& index : llvm::make_range(inst->idx_begin(), inst->idx_end())) {
#endif
    LOG_DEBUG("Iter " << skip_first << " with " << *index.get())
    if (skip_first) {
      skip_first = false;
      continue;
    }
    if (auto* const_idx = llvm::dyn_cast<llvm::ConstantInt>(index.get())) {
      const int64_t index = const_idx->getValue().getSExtValue();
      gep_ind.indices_.emplace_back(index);
    }
  }
  return gep_ind;
}

inline llvm::raw_ostream& operator<<(llvm::raw_ostream& os, const GepIndices& indices) {
  const auto& vec = indices.indices_;
  if (vec.empty()) {
    os << "[]";
    return os;
  }
  const auto* begin = std::begin(vec);
  os << "[" << *begin;
  std::for_each(std::next(begin), std::end(vec), [&](const auto value) {
    os << ", ";
    os << value;
  });
  os << "]";
  return os;
}

llvm::DINode* select_non_zero_element(llvm::DINode* element, llvm::DINode* next_element) {
  // used to detect the empty base class optimization
  auto* derived_type_member      = llvm::dyn_cast<llvm::DIDerivedType>(element);
  auto* next_derived_type_member = llvm::dyn_cast<llvm::DIDerivedType>(next_element);
  if (derived_type_member != nullptr && next_derived_type_member != nullptr) {
    LOG_DEBUG("Non-null elements")
    if (derived_type_member->getOffsetInBits() == next_derived_type_member->getOffsetInBits()) {
      LOG_DEBUG("Same offset detected: " << log::ditype_str(derived_type_member) << " and "
                                         << log::ditype_str(next_derived_type_member))
      return next_element;
    }
  }
  return element;
}

namespace detail {

template <typename UnlessFn>
auto find_non_derived_type_unless(llvm::DIType* root, UnlessFn&& unless) {
  llvm::DIType* type = root;
  while (type && llvm::isa<llvm::DIDerivedType>(type)) {
    if (unless(type)) {
      break;
    }
    auto* ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
    type         = ditype->getBaseType();
  }
  return type;
}

inline bool is_pointer_like(const llvm::DIType& di_type) {
  if (const auto* type = llvm::dyn_cast<llvm::DIDerivedType>(&di_type)) {
    return type->getTag() == llvm::dwarf::DW_TAG_array_type || type->getTag() == llvm::dwarf::DW_TAG_reference_type ||
           type->getTag() == llvm::dwarf::DW_TAG_pointer_type ||
           type->getTag() == llvm::dwarf::DW_TAG_ptr_to_member_type;
  }
  return false;
}

}  // namespace detail

auto find_non_derived_type_unless_ptr(llvm::DIType* root) {
  return detail::find_non_derived_type_unless(root, [](auto* val) { return detail::is_pointer_like(*val); });
}

auto find_non_derived_type(llvm::DIType* root) {
  return detail::find_non_derived_type_unless(root, [](auto* val) { return false; });
}

llvm::DICompositeType* skip_first_gep_access(llvm::DICompositeType* composite_type) {
  const auto select_next_member = [&](llvm::DICompositeType* base) -> std::optional<llvm::DIType*> {
    auto elems = base->getElements();
    if (elems.empty()) {
      return {};
    }
    auto* element = *base->getElements().begin();
    if (elems.size() > 1) {
      auto* next_element = *(std::next(base->getElements().begin()));
      element            = select_non_zero_element(element, next_element);
    }

    return find_non_derived_type_unless_ptr(llvm::dyn_cast<llvm::DIType>(element));
  };

  const auto should_iterate_next_member = [](auto* composite_type) {
    auto elem_count = llvm::count_if(composite_type->getElements(),
                                     [](const auto& elem) { return !llvm::isa<llvm::DISubprogram>(elem); });
    if (elem_count > 1) {
      auto element      = *composite_type->getElements().begin();
      auto next_element = *(std::next(composite_type->getElements().begin()));
      element           = select_non_zero_element(element, next_element);
      return element == next_element;
    }

    return elem_count == 1;
  };

  while (should_iterate_next_member(composite_type)) {
    auto next_di = select_next_member(composite_type);
    if (!next_di) {
      break;
    }
    if (!llvm::isa<llvm::DICompositeType>(next_di.value())) {
      break;
    }
    composite_type = llvm::dyn_cast<llvm::DICompositeType>(next_di.value());
  }

  return composite_type;
}

GepIndexToType iterate_gep_index(llvm::DICompositeType* composite_type, const GepIndices& gep_indices) {
  const auto has_next_idx = [&gep_indices](size_t pos) { return pos + 1 < gep_indices.size(); };

  LOG_DEBUG("Iterate over gep: " << gep_indices);

  for (const auto& enum_index : llvm::enumerate(gep_indices.indices())) {
    const auto index  = enum_index.value();
    const auto& elems = composite_type->getElements();
    assert(elems.size() > index);

    auto* element = elems[index];
    if (!llvm::isa<llvm::DIDerivedType>(element)) {
      LOG_DEBUG("Index shows to non-derived type: " << log::ditype_str(element))
      // TODO, if only one index, and this triggers, go first element all the way down?
      // maybe also check for class type (not structs etc.)
    }

    if (index == 0 && elems.size() > 1) {
      LOG_DEBUG("Check zero-size pattern")
      auto* next_element = elems[index + 1];
      element            = select_non_zero_element(element, next_element);
    }

    LOG_DEBUG(" element: " << log::ditype_str(element))

    if (auto* derived_type_member = llvm::dyn_cast<llvm::DIDerivedType>(element)) {
      auto* member_type = find_non_derived_type_unless_ptr(derived_type_member->getBaseType());

      LOG_DEBUG("Looking at " << log::ditype_str(member_type))

      if (auto* composite_member_type = llvm::dyn_cast<llvm::DICompositeType>(member_type)) {
        if (composite_member_type->getTag() == llvm::dwarf::DW_TAG_class_type ||
            composite_member_type->getTag() == llvm::dwarf::DW_TAG_structure_type) {
          // maybe need to recurse into!
          if (has_next_idx(enum_index.index())) {
            composite_type = composite_member_type;
            continue;
          }
        }
        if (composite_member_type->getTag() == llvm::dwarf::DW_TAG_array_type) {
          if (has_next_idx(enum_index.index())) {
            // At end of gep instruction, return basetype:
            return GepIndexToType{composite_member_type->getBaseType(), derived_type_member};
          }
          // maybe need to recurse into tag_array_type (of non-basic type...)
        }
      }

      return GepIndexToType{member_type, derived_type_member};
    }
  }
  return {};
}

struct DestructureGepIndex : visitor::DINodeVisitor<DestructureGepIndex> {
  explicit DestructureGepIndex(const size_t index) : byte_index_{index} {
  }

  [[nodiscard]] std::optional<GepIndexToType> result() const {
    return this->outermost_candidate_;
  }

  bool visitCompositeType(const llvm::DICompositeType* composite) const {
    LOG_DEBUG("visitCompositeType: " << composite->getName() << " index: " << byte_index_
                                     << " offset base: " << this->offset_base_);
    return true;
  }

  bool visitDerivedType(const llvm::DIDerivedType* derived_ty) {
    if (derived_ty->getTag() != llvm::dwarf::DW_TAG_member) {
      return true;
    }
    // assert(derived_ty->getTag() == llvm::dwarf::DW_TAG_member && "Expected member element in composite ty");
    LOG_DEBUG("looking @ member: " << derived_ty->getName() << " offset: " << derived_ty->getOffsetInBits() / 8
                                   << " size: " << derived_ty->getSizeInBits() / 8);

    const auto deriv_offset = (derived_ty->getOffsetInBits() / 8);
    const auto deriv_size   = (derived_ty->getSizeInBits() / 8);
    const auto offset       = this->offset_base_ + deriv_offset;
    const auto lower_bound  = offset;
    const auto upper_bound  = offset + deriv_size;

    if (byte_index_ >= lower_bound && byte_index_ < upper_bound) {
      auto* const member_base_type = derived_ty->getBaseType();

      LOG_DEBUG("saving candidate member type " << log::ditype_str(member_base_type));

      this->outermost_candidate_.emplace(
          GepIndexToType{member_base_type, const_cast<llvm::DIDerivedType*>(derived_ty)});

      if (detail::is_pointer_like(*member_base_type) || member_base_type->getTag() == llvm::dwarf::DW_TAG_array_type) {
        return false;  // if offset matches, and its a pointer-like, we do not need to recurse.
      }

      // We should only ever be able to recurse into one composite type where the offset condition holds, so
      // save the offset base for that member.
      if (llvm::isa<llvm::DICompositeType>(member_base_type)) {
        LOG_DEBUG("setting offset base to: " << offset);
        this->offset_base_ = offset;
      }
    }
    return true;
  }

 private:
  size_t byte_index_;
  size_t offset_base_{};
  std::optional<GepIndexToType> outermost_candidate_{};
};

GepIndexToType resolve_gep_index_to_type(llvm::DICompositeType* composite_type, const GepIndices& gep_indices) {
  if (gep_indices.empty()) {
    // this triggers for composite (-array) access without constant index, see "heap_milc_struct_mock.c":
    LOG_DEBUG("Gep indices empty")
    return GepIndexToType{composite_type};
  }

  if (gep_indices.skipped_first() && gep_indices.indices_[0] != 0) {
    // This assumes that a single (and only single) first 0 skips through to the first element with more than one
    // member: struct A { struct B { struct C { int, int } } } -> would skip to "struct C" for gep [0 1]
    LOG_DEBUG("Skip single member nested of: " << log::ditype_str(composite_type))
    composite_type = skip_first_gep_access(composite_type);
    LOG_DEBUG("Result of skip: " << log::ditype_str(composite_type))
  }

  if (gep_indices.byte_access()) {
    LOG_DEBUG("Trying to resolve byte access based on offset " << gep_indices.indices_[0])
    DestructureGepIndex visitor{gep_indices.indices_[0]};
    visitor.traverseCompositeType(composite_type);

    // TODO: Should this function really return `GepIndexToType` instead of an optional?
    return visitor.result().value_or(GepIndexToType{composite_type});
  }

  return iterate_gep_index(composite_type, gep_indices);
}

std::optional<GepIndexToType> try_resolve_inlined_operator(const llvm::GEPOperator* gep) {
  const auto* const load = llvm::dyn_cast<llvm::Instruction>(gep->getPointerOperand());
  if (!load) {
    return {};
  }

  const auto* const sub_prog = llvm::dyn_cast<llvm::DISubprogram>(load->getDebugLoc().getScope());
  assert(sub_prog && "Scope does not represent a subprogram");

  // see cpp/heap_vector_operator.cpp: vector::operator[] returns a reference, that we skip here:
  const auto remove_ref = [&](auto* di_type) {
    auto node = detail::find_non_derived_type_unless(di_type, [](auto* node) {
      if (const auto* type = llvm::dyn_cast<llvm::DIDerivedType>(node)) {
        return type->getTag() == llvm::dwarf::DW_TAG_pointer_type ||
               type->getTag() == llvm::dwarf::DW_TAG_ptr_to_member_type;
      }
      return false;
    });
    return node;
  };

  if (auto* sub_program_type = sub_prog->getType()) {
    if (sub_program_type->getTypeArray().size() > 0) {
      return {GepIndexToType{remove_ref(*sub_program_type->getTypeArray().begin())}};
    }
  }
  LOG_DEBUG("Could not detect inlined operator")
  return {};
}

GepIndexToType extract_gep_dereferenced_type(llvm::DIType* root, const llvm::GEPOperator& inst) {
  using namespace llvm;

  auto* const gep_src = inst.getSourceElementType();

  auto* const base_ty        = find_non_derived_type(root);
  auto* const composite_type = llvm::dyn_cast_or_null<DICompositeType>(base_ty);
  // see test cpp/heap_vector_opt.cpp: GEP on pointer (of inlined operator[])
  const bool may_be_inlined_operator = (composite_type != nullptr) && composite_type->isForwardDecl();

  if (gep_src->isPointerTy() && !may_be_inlined_operator) {
    LOG_DEBUG("Gep to ptr " << log::ditype_str(root));
    return GepIndexToType{root};
  }

  if (gep_src->isArrayTy()) {
    if (composite_type != nullptr) {
      auto* base_type = composite_type->getBaseType();
      LOG_DEBUG("Gep to array of DI composite, with base type " << log::ditype_str(base_type));
      return GepIndexToType{base_type};
    }
    LOG_DEBUG("Gep to array " << log::ditype_str(root));
    return GepIndexToType{root};
  }

  auto* const derived_root = llvm::dyn_cast<DIDerivedType>(root);
  // TODO: This check seems like a bad idea but I'm not really sure how to do it properly, I reckon we need *some*
  //       heuristic to detect "fake-array" types though (e.g. gep/array_composite_s.c)
  if (util::is_byte_indexing(&inst) &&
      (!composite_type || (derived_root && derived_root->getBaseType()->getTag() == dwarf::DW_TAG_pointer_type))) {
    return GepIndexToType{root};
  }

  assert(composite_type != nullptr && "Root should be a struct-like type.");

  if (composite_type->isForwardDecl()) {
    LOG_DEBUG("Trying to resolve forward-declared composite type " << log::ditype_str(composite_type))
    return try_resolve_inlined_operator(&inst).value_or(GepIndexToType{});
  }

  LOG_DEBUG("Gep to DI composite: " << log::ditype_str(composite_type))
  bool skip_first{!util::is_first_non_zero_indexing(&inst)};
  if (util::is_byte_indexing(&inst)) {
    LOG_DEBUG("Access based on i8 ptr, assuming byte offsetting into composite member")
    skip_first = false;  // We do not skip over byte index values (likely != 0)
  }

  auto accessed_ditype = resolve_gep_index_to_type(composite_type, GepIndices::create(&inst, skip_first));

  return accessed_ditype;
}

}  // namespace dimeta::gep
