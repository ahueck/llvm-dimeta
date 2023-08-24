//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "GEP.h"

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
  using Iter = llvm::SmallVector<uint64_t, 4>::const_iterator;

  inline llvm::iterator_range<Iter> indices() const {
    return llvm::iterator_range<Iter>(indices_);
  }

  inline size_t size() const {
    return indices_.size();
  }

  inline bool skipped_first() const {
    return skipped;
  }

  static GepIndices create(const llvm::GEPOperator* inst, bool skip_first = true);
};

GepIndices GepIndices::create(const llvm::GEPOperator* inst, bool skip_first) {
  GepIndices gep_ind;
  gep_ind.gep     = inst;
  gep_ind.skipped = skip_first;

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

std::optional<llvm::DIType*> resolve_gep_index_to_type(llvm::DICompositeType* composite_type, const GepIndices& inst) {
  if (inst.skipped_first() && inst.indices_[0] != 0) {
    // This assumes that a single (and only single) first 0 skips through to the first element with more than one
    // member: struct A { struct B { struct C { int, int } } } -> would skip to "struct C" for gep [0 1]
    llvm::dbgs() << "IN: " << *composite_type << "-> ";
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
    llvm::dbgs() << *composite_type << "\n";
  }

  const auto has_next_idx = [&inst](size_t pos) { return pos + 1 < inst.size(); };
  llvm::dbgs() << "Gep: " << inst << "\n";
  for (const auto& enum_index : llvm::enumerate(inst.indices())) {
    const auto index  = enum_index.value();
    const auto& elems = composite_type->getElements();
    assert(elems.size() > index);

    auto element = elems[index];

    llvm::dbgs() << " element: " << *element << "\n";

    if (auto derived_type = llvm::dyn_cast<llvm::DIDerivedType>(element)) {
      assert(derived_type->getTag() == llvm::dwarf::DW_TAG_member);
      auto member_type = derived_type->getBaseType();

      if (auto composite_member_type = llvm::dyn_cast<llvm::DICompositeType>(member_type)) {
        if (composite_member_type->getTag() == llvm::dwarf::DW_TAG_class_type ||
            composite_member_type->getTag() == llvm::dwarf::DW_TAG_structure_type) {
          // maybe need to recurse into!
          if (has_next_idx(enum_index.index())) {
            llvm::dbgs() << "Has next\n";
            //            llvm::dbgs() << "C " << index_counter << " s " << inst.size() << "\n";
            composite_type = composite_member_type;
            continue;
          }
        }
        if (composite_member_type->getTag() == llvm::dwarf::DW_TAG_array_type) {
          if (has_next_idx(enum_index.index())) {
            // At end of gep instruction, return basetype:
            //            llvm::dbgs() << " return base-type\n";
            return composite_member_type->getBaseType();
          }
          // maybe need to recurse into tag_array_type (of non-basic type...)
        }
      }

      return member_type;
    }
  }
  return {};
}

std::optional<llvm::DIType*> extract_gep_deref_type(llvm::DIType* root, const llvm::GEPOperator& inst) {
  using namespace llvm;

  auto gep_src = inst.getSourceElementType();
  if (gep_src->isPointerTy()) {
    llvm::dbgs() << "Gep to ptr\n";
    return root;  // basetype
  }

  if (gep_src->isArrayTy()) {
    llvm::dbgs() << "Gep to array\n";
    if (auto composite_type = llvm::dyn_cast<llvm::DICompositeType>(root)) {
      return composite_type->getBaseType();
    }
    return root;
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

  auto accessed_ditype = resolve_gep_index_to_type(composite_type, GepIndices::create(&inst));

  return accessed_ditype;
}

}  // namespace dimeta::gep