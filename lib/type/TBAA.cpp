//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "TBAA.h"

#include "DefUseAnalysis.h"
#include "support/Logger.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Support/Casting.h"

#include <assert.h>
#include <string>

namespace dimeta::tbaa {

struct TBAAHandle {
  llvm::MDNode* base_ty;
  llvm::MDNode* access_ty;
  llvm::ConstantInt* offset;

  static std::optional<TBAAHandle> create(const llvm::StoreInst& store) {
    auto* access = store.getMetadata(llvm::StringRef{"tbaa"});
    if (!access) {
      return {};
    }

    return TBAAHandle(*access);
  }

  llvm::StringRef base_name() const {
    return llvm::dyn_cast<llvm::MDString>(base_ty->getOperand(0))->getString();
  }

  llvm::StringRef access_name() const {
    return llvm::dyn_cast<llvm::MDString>(access_ty->getOperand(0))->getString();
  }

  bool access_is_ptr() const {
    return access_name() == "any pointer";
  }

  bool base_is_ptr() const {
    return base_name() == "any pointer";
  }

 private:
  explicit TBAAHandle(llvm::MDNode& tbaa_node) {
    base_ty       = llvm::dyn_cast<llvm::MDNode>(tbaa_node.getOperand(0));
    access_ty     = llvm::dyn_cast<llvm::MDNode>(tbaa_node.getOperand(1));
    auto value_md = llvm::dyn_cast<llvm::ValueAsMetadata>(tbaa_node.getOperand(2))->getValue();
    assert(value_md && "Offset value may not be null.");
    offset = llvm::dyn_cast<llvm::ConstantInt>(value_md);
  }
};

inline std::string tbaa_operand_name(llvm::MDNode* type_node) {
  auto string_node = llvm::dyn_cast<llvm::MDString>(type_node->getOperand(0));
  assert(string_node);
  return std::string{string_node->getString()};
}

inline bool tbaa_operand_is_ptr(llvm::MDNode* type_node) {
  return tbaa_operand_name(type_node) == "any pointer";
}

std::optional<llvm::DIType*> resolve_tbaa(llvm::DIType* root, const dataflow::ValuePath& path) {
  using namespace tbaa;
  auto value = llvm::dyn_cast<llvm::StoreInst>(path.start_value());
  if (!value) {
    return root;
  }
  assert(value != nullptr && "Last value should be a store instruction.");

  LOG_DEBUG("Resolve TBAA of ditype: " << log::ditype_str(root))

  auto tbaa = TBAAHandle::create(*value);
  assert(tbaa.has_value() && "Requires TBAA.");

  // assign any ptr to any ptr, e.g., struct A** a; a[0] = malloc(struct A):
  if (tbaa->base_ty == tbaa->access_ty && tbaa->access_is_ptr()) {
    LOG_DEBUG("No work: TBAA base type is same as access type (both ptr).")
    return root;
  }

  const auto find_composite = [](llvm::DIType* root) {
    llvm::DIType* type = root;
    while (type && llvm::isa<llvm::DIDerivedType>(type)) {
      auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
      type        = ditype->getBaseType();
    }
    return type;
  };

  auto maybe_composite = find_composite(root);
  if (!maybe_composite || !llvm::isa<llvm::DICompositeType>(maybe_composite)) {
    LOG_DEBUG("Returning: DIType is not composite required for TBAA descend.")
    return root;
  }
  auto composite = llvm::dyn_cast<llvm::DICompositeType>(maybe_composite);

  LOG_DEBUG("Found legible composite node: " << log::ditype_str(composite))

  //  assert(root->getTag() == llvm::dwarf::DW_TAG_structure_type && "Root should be struct-like");

  auto struct_name = std::string{composite->getName()};
  assert(struct_name == tbaa->base_name() && "Root DIType should have same struct name.");

  // Handle "malloc" -> "store" (to struct) at offset 0 (optimized away gep):
  assert(tbaa->access_is_ptr());
  const auto next_di_member = [&](llvm::DICompositeType* base) -> std::optional<llvm::DIType*> {
    if (base->getElements().empty()) {
      return {};
    }
    auto node = *base->getElements().begin();
    return find_composite(llvm::dyn_cast<llvm::DIType>(node));
  };

  const auto next_tbaa_type = [](llvm::MDNode* base_ty) -> llvm::MDNode* {
    // return operand at 1 (the base_ty member at offset 0)
    return llvm::dyn_cast<llvm::MDNode>(base_ty->getOperand(1));
  };

  bool endpoint_reached = false;
  auto next_tbaa        = tbaa->base_ty;
  auto next_ditype      = composite;
  LOG_DEBUG("TBAA tree iteration.")
  LOG_DEBUG("  From ditype: " << log::ditype_str(composite))
  LOG_DEBUG("  From tbaa: " << log::ditype_str(next_tbaa))
  do {
    next_tbaa        = next_tbaa_type(next_tbaa);
    auto try_next_di = next_di_member(next_ditype);

    endpoint_reached = tbaa_operand_is_ptr(next_tbaa);
    if (endpoint_reached) {
      return try_next_di.value();
    } else {
      // not yet found the any_pointer, hence recurse!
      next_ditype = llvm::dyn_cast<llvm::DICompositeType>(try_next_di.value());
      assert(next_ditype && "We expect a composite type here");
    }
  } while (!endpoint_reached);

  return next_ditype;
}

}  // namespace dimeta::tbaa
