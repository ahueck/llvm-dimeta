//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "GEP.h"

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
  gep_ind.is_byte_access = gep_ind.gep->getSourceElementType()->isIntegerTy(8);

#if LLVM_VERSION_MAJOR > 12
  for (const auto& index : inst->indices()) {
#else
  for (const auto& index : llvm::make_range(inst->idx_begin(), inst->idx_end())) {
#endif
    if (skip_first) {
      skip_first = false;
      continue;
    }
    if (auto const_idx = llvm::dyn_cast<llvm::ConstantInt>(index.get())) {
      int64_t index = const_idx->getValue().getSExtValue();
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
    const auto& elems = composite_type->getElements();
    assert(elems.size() > 0 && "Need at least one member for gep byte-based access");
    assert(gep_indices.size() == 1 && "Byte access is only supported for one byte index value");
    for (auto element : elems) {
      if (auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(element)) {
        const auto offset_bytes = ditype->getOffsetInBits() / 8;
        if (offset_bytes == gep_indices.indices_[0]) {
          return GepIndexToType{ditype->getBaseType(), ditype};
        }
      }
    }
  }

  if (gep_indices.skipped_first() && gep_indices.indices_[0] != 0) {
    // This assumes that a single (and only single) first 0 skips through to the first element with more than one
    // member: struct A { struct B { struct C { int, int } } } -> would skip to "struct C" for gep [0 1]
    LOG_DEBUG("Skip single member nested of: " << log::ditype_str(composite_type))
    const auto find_composite = [](llvm::DIType* root) {
      llvm::DIType* type = root;
      while (type && llvm::isa<llvm::DIDerivedType>(type)) {
        auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
        type        = ditype->getBaseType();
      }
      return type;
    };

    const auto next_di_member = [&](llvm::DICompositeType* base) -> std::optional<llvm::DIType*> {
      if (base->getElements().empty()) {
        return {};
      }
      auto node = *base->getElements().begin();
      return find_composite(llvm::dyn_cast<llvm::DIType>(node));
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

  const auto has_next_idx = [&gep_indices](size_t pos) { return pos + 1 < gep_indices.size(); };
  LOG_DEBUG("Iterate over gep: " << gep_indices);
  for (const auto& enum_index : llvm::enumerate(gep_indices.indices())) {
    const auto index  = enum_index.value();
    const auto& elems = composite_type->getElements();
    assert(elems.size() > index);

    auto element = elems[index];

    LOG_DEBUG(" element: " << log::ditype_str(element))

    if (auto derived_type_member = llvm::dyn_cast<llvm::DIDerivedType>(element)) {
      assert(derived_type_member->getTag() == llvm::dwarf::DW_TAG_member && "Expected member tag");
      auto member_type = derived_type_member->getBaseType();

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

  auto gep_src = inst.getSourceElementType();

  if (gep_src->isPointerTy()) {
    LOG_DEBUG("Gep to ptr " << log::ditype_str(root));
    // The commented code is used in conjunction with load/store reset (non-basic!, e.g., reset_load_related):
    //    if (auto* type_behind_ptr = llvm::dyn_cast<llvm::DIDerivedType>(root)) {
    //      assert((type_behind_ptr->getTag() == llvm::dwarf::DW_TAG_pointer_type) && "Expected a DI pointer type.");
    //      return type_behind_ptr->getBaseType();
    //    }
    return GepIndexToType{root};
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
    llvm::DIType* type = root;
    while (llvm::isa<llvm::DIDerivedType>(type)) {
      auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
      type        = ditype->getBaseType();
    }
    return type;
  };

  auto composite_type = llvm::dyn_cast<llvm::DICompositeType>(find_composite(root));
  assert(composite_type != nullptr && "Root should be a struct-like type.");

  LOG_DEBUG("Gep to DI composite: " << log::ditype_str(composite_type))
  bool skip_first = !gep_src->isIntegerTy(8);
  if (skip_first) {
    LOG_DEBUG("Access based on i8 ptr")
  }
  auto accessed_ditype = resolve_gep_index_to_type(composite_type, GepIndices::create(&inst, skip_first));

  return accessed_ditype;
}

}  // namespace dimeta::gep