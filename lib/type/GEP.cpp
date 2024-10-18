//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
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
  if (auto const_idx = llvm::dyn_cast<llvm::ConstantInt>((*gep->idx_begin()).get())) {
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
    if (auto const_idx = llvm::dyn_cast<llvm::ConstantInt>(index.get())) {
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

GepIndexToType resolve_gep_index_to_type(llvm::DICompositeType* composite_type, const GepIndices& gep_indices) {
  if (gep_indices.empty()) {
    // this triggers for composite (-array) access without constant index, see "heap_milc_struct_mock.c":
    LOG_DEBUG("Gep indices empty")
    return GepIndexToType{composite_type};
  }

  if (gep_indices.byte_access()) {
    // Test heap_tachyon_mock_image.c for llvm 12:
    // This mostly applies to llvm <= 12?
    LOG_DEBUG("Gep indices are byte access: " << gep_indices)
    const auto& elems = composite_type->getElements();
    assert(elems.size() > 0 && "Need at least one member for gep byte-based access");
    assert(gep_indices.size() == 1 && "Byte access is only supported for one byte index value");
    for (auto element : elems) {
      if (auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(element)) {
        assert(ditype->getTag() == llvm::dwarf::DW_TAG_member && "A member is expected here");
        const auto offset_bytes = ditype->getOffsetInBits() / 8;
        if (offset_bytes == gep_indices.indices_[0]) {
          return GepIndexToType{ditype->getBaseType(), ditype};
        }
      }
    }
  }

  const auto find_non_derived_type = [](llvm::DIType* root) {
    llvm::DIType* type = root;
    while (type && llvm::isa<llvm::DIDerivedType>(type)) {
      auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
      type        = ditype->getBaseType();
    }
    return type;
  };

  if (gep_indices.skipped_first() && gep_indices.indices_[0] != 0) {
    // This assumes that a single (and only single) first 0 skips through to the first element with more than one
    // member: struct A { struct B { struct C { int, int } } } -> would skip to "struct C" for gep [0 1]
    LOG_DEBUG("Skip single member nested of: " << log::ditype_str(composite_type))

    const auto next_di_member = [&](llvm::DICompositeType* base) -> std::optional<llvm::DIType*> {
      if (base->getElements().empty()) {
        return {};
      }
      auto node = *base->getElements().begin();
      return find_non_derived_type(llvm::dyn_cast<llvm::DIType>(node));
    };
    while (composite_type->getElements().size() == 1) {
      auto next_di = next_di_member(composite_type);
      if (!next_di) {
        break;
      }
      composite_type = llvm::dyn_cast<llvm::DICompositeType>(next_di.value());
    }
    LOG_DEBUG("Result of skip: " << log::ditype_str(composite_type))
  }

  // visitor::util::print_dinode(composite_type, llvm::outs());

  const auto has_next_idx = [&gep_indices](size_t pos) { return pos + 1 < gep_indices.size(); };
  LOG_DEBUG("Iterate over gep: " << gep_indices);
  for (const auto& enum_index : llvm::enumerate(gep_indices.indices())) {
    const auto index  = enum_index.value();
    const auto& elems = composite_type->getElements();
    assert(elems.size() > index);

    auto element = elems[index];
    if (!llvm::isa<llvm::DIDerivedType>(element)) {
      LOG_DEBUG("Index shows to non-derived type: " << *element)
      // TODO, if only one index, and this triggers, go first element all the way down?
      // maybe also check for class type (not structs etc.)
    }

    // if (index == 0 && elems.size() > 1) {
    //   // std::exit(1);
    //   LOG_DEBUG("Check zero-size pattern")
    //   auto next_element             = elems[index + 1];
    //   auto derived_type_member      = llvm::dyn_cast<llvm::DIDerivedType>(element);
    //   auto next_derived_type_member = llvm::dyn_cast<llvm::DIDerivedType>(next_element);
    //   if (derived_type_member->getOffsetInBits() == next_derived_type_member->getOffsetInBits()) {
    //     LOG_DEBUG(*derived_type_member << " same offset as " << *next_derived_type_member)
    //   }
    // }

    LOG_DEBUG(" element: " << log::ditype_str(element))

    if (auto derived_type_member = llvm::dyn_cast<llvm::DIDerivedType>(element)) {
      auto member_type = [&find_non_derived_type](auto type) -> llvm::DIType* {
        // auto derived = llvm::dyn_cast<llvm::DIDerivedType>(type);
        // if (derived && derived->getTag() == llvm::dwarf::DW_TAG_inheritance) {
        //   return derived->getBaseType();
        // }

        return find_non_derived_type(type);
        // return type;
      }(derived_type_member->getBaseType());

      LOG_DEBUG("Looking at " << log::ditype_str(member_type))

      if (auto composite_member_type = llvm::dyn_cast<llvm::DICompositeType>(member_type)) {
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

GepIndexToType extract_gep_deref_type(llvm::DIType* root, const llvm::GEPOperator& inst) {
  using namespace llvm;

  const auto gep_src = inst.getSourceElementType();

  if (gep_src->isPointerTy()) {
    LOG_DEBUG("Gep to ptr " << log::ditype_str(root));
    // The commented code is used in conjunction with load/store reset (non-basic!, e.g., reset_load_related):
    //    if (auto* type_behind_ptr = llvm::dyn_cast<llvm::DIDerivedType>(root)) {
    //      assert((type_behind_ptr->getTag() == llvm::dwarf::DW_TAG_pointer_type) && "Expected a DI pointer type.");
    //      return type_behind_ptr->getBaseType();
    //    }
    if (!llvm::isa<llvm::DICompositeType>(root)) {
      return GepIndexToType{root};
    }
  }

  if (gep_src->isArrayTy()) {
    if (auto composite_type = llvm::dyn_cast<llvm::DICompositeType>(root)) {
      auto base_type = composite_type->getBaseType();
      LOG_DEBUG("Gep to array of DI composite, with base type " << log::ditype_str(base_type));
      return GepIndexToType{base_type};
    }
    LOG_DEBUG("Gep to array " << log::ditype_str(root));
    return GepIndexToType{root};
  }

  const auto find_composite = [](llvm::DIType* root) {
    assert(root != nullptr && "Root type should be non-null");
    llvm::DIType* type = root;
    while (llvm::isa<llvm::DIDerivedType>(type)) {
      const auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
      auto next_type    = ditype->getBaseType();
      if (next_type != nullptr) {
        type = next_type;
      } else {
        break;
      }
    }
    return type;
  };

  const auto composite_type = llvm::dyn_cast<llvm::DICompositeType>(find_composite(root));
  assert(composite_type != nullptr && "Root should be a struct-like type.");

  if (composite_type->isForwardDecl()) {
    LOG_DEBUG("Forward declared composite type cannot be resolved " << log::ditype_str(composite_type))
    // TODO make some error code for such a case
    return GepIndexToType{};
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