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
#include "llvm/BinaryFormat/Dwarf.h"
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
  auto store_inst = llvm::dyn_cast<llvm::StoreInst>(path.start_value());
  if (!store_inst) {
    return root;
  }
  assert(store_inst != nullptr && "Last value in path should be a store instruction.");

  LOG_DEBUG("Resolve TBAA of store '" << *store_inst << "' with ditype: " << log::ditype_str(root))

  auto tbaa = TBAAHandle::create(*store_inst);
  assert(tbaa.has_value() && "Requires TBAA metadata in LLVM IR.");

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

  const auto find_first_non_member = [](llvm::DIType* root) {
    llvm::DIType* type = root;
    while (type && llvm::isa<llvm::DIDerivedType>(type)) {
      auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
      if (ditype->getTag() != llvm::dwarf::DW_TAG_member) {
        break;
      }
      type = ditype->getBaseType();
    }
    return type;
  };

  auto maybe_composite = find_composite(root);
  if (!maybe_composite || !llvm::isa<llvm::DICompositeType>(maybe_composite)) {
    LOG_DEBUG("Returning: DIType is not composite required for TBAA descend.")
    return root;
  }
  auto composite = llvm::dyn_cast<llvm::DICompositeType>(maybe_composite);

  LOG_DEBUG("Found maybe legible composite node: " << log::ditype_str(composite))

  //  assert(root->getTag() == llvm::dwarf::DW_TAG_structure_type && "Root should be struct-like");

  // Cpp TBAA uses identifier, in C we use the name:
  auto struct_name = std::string{composite->getIdentifier()};
  if (struct_name.empty()) {
    struct_name = std::string{composite->getName()};
  }

  if (struct_name != tbaa->base_name()) {
    LOG_DEBUG("Return, names differ. Name of struct: " << struct_name << " vs name of TBAA " << tbaa->base_name())
    return root;
  }

  const auto num_members_tbaa_node = [](llvm::MDNode* node) {
    const auto num = node->getNumOperands();
    assert(num > 0 && "Operand count must be > 0");
    return (num - 1) / 2;
  };

  if (struct_name.empty()) {
    // Here no name matching is possible, is TBAA node referring to current DI type? We need to verify:
    const auto composite_fits_tbaa = [&](const llvm::DICompositeType* type) {
      if (type->getElements().size() != num_members_tbaa_node(tbaa->base_ty)) {
        return false;
      }
      auto elements = type->getElements();
      int position  = 0;
      // Loop simply checks if the byte offsets are the same (TODO also compare types!)
      for (auto& operand : tbaa->base_ty->operands()) {
        if (auto value_md = llvm::dyn_cast<llvm::ValueAsMetadata>(operand)) {
          if (auto current_offset = llvm::dyn_cast<llvm::ConstantInt>(value_md->getValue())) {
            auto current_member = llvm::dyn_cast<llvm::DIDerivedType>(elements[position]);
            if (!current_offset->equalsInt(current_member->getOffsetInBits() / 8)) {
              return false;
            }
          }
        }
        ++position;
      }

      return true;
    };
    if (!composite_fits_tbaa(composite)) {
      LOG_DEBUG("TBAA of anonymous struct has different offsets to DIComposite type!")
      return root;
    }
  }

  //  assert(struct_name == tbaa->base_name() && "Root DIType should have same struct name.");

  // Handle "malloc" -> "store" (to struct) at offset 0 (optimized away gep):
  assert(tbaa->access_is_ptr());

  const auto next_di_member = [&](llvm::DICompositeType* base, int member_index = 0) -> std::optional<llvm::DIType*> {
    if (base->getElements().empty()) {
      return {};
    }
    if (member_index == 0) {
      auto node = *base->getElements().begin();
      // this ignores pointer types etc.:    return find_composite(llvm::dyn_cast<llvm::DIType>(node));
      return find_first_non_member(llvm::dyn_cast<llvm::DIType>(node));
    }
    assert(member_index < base->getElements().size() && "Member index value needs to be within number of DI members!");
    auto node = base->getElements()[member_index];
    LOG_DEBUG("Next DIType member at pos " << member_index << " is " << log::ditype_str(node))
    // this ignores pointer types etc.:    return find_composite(llvm::dyn_cast<llvm::DIType>(node));
    return find_first_non_member(llvm::dyn_cast<llvm::DIType>(node));
  };

  const auto next_tbaa_type = [](llvm::MDNode* base_ty, llvm::ConstantInt* offset) -> std::pair<llvm::MDNode*, int> {
    // return operand at 1 (the base_ty member at offset 0)
    if (!offset) {
      return {llvm::dyn_cast<llvm::MDNode>(base_ty->getOperand(1)), 0};
    }
    auto pos = llvm::find_if(base_ty->operands(), [&](const llvm::MDOperand& operand) {
      if (auto value_md = llvm::dyn_cast<llvm::ValueAsMetadata>(operand)) {
        if (auto current_offset = llvm::dyn_cast<llvm::ConstantInt>(value_md->getValue())) {
          if (offset->getValue() == current_offset->getValue()) {
            return true;
          }
        }
      }
      return false;
    });
    assert(pos != std::end(base_ty->operands()) && "Could not find offset for access of type!");
    auto operand_node_at_offset = std::prev(pos);
    auto dist                   = std::distance(std::begin(base_ty->operands()), pos);
    int offset_member_access    = static_cast<int>(dist) / 2 - 1;
    assert(offset_member_access >= 0 && "TBAA access index to DIType must be positive number!");
    return {llvm::dyn_cast<llvm::MDNode>(*operand_node_at_offset), offset_member_access};
  };

  bool endpoint_reached = false;
  auto next_tbaa        = tbaa->base_ty;
  auto next_ditype      = composite;
  auto next_offset      = tbaa->offset;
  LOG_DEBUG("TBAA tree iteration.")
  LOG_DEBUG("  From ditype: " << log::ditype_str(composite))
  LOG_DEBUG("  From TBAA: " << log::ditype_str(next_tbaa))
  do {
    auto [next_tbaa_node, access_offset_index] = next_tbaa_type(next_tbaa, next_offset);
    next_tbaa                                  = next_tbaa_node;
    auto try_next_di                           = next_di_member(next_ditype, access_offset_index);

    next_offset = nullptr;

    endpoint_reached = tbaa_operand_is_ptr(next_tbaa);

    LOG_DEBUG("  >> New ditype: " << log::ditype_str(try_next_di.value()))
    LOG_DEBUG("  >> New TBAA: " << log::ditype_str(next_tbaa))
    if (endpoint_reached) {
      LOG_DEBUG("  >> Endpoint " << log::ditype_str(try_next_di.value()))
      return try_next_di.value();
    } else {
      // not yet found the any_pointer, hence recurse!
      next_ditype = llvm::dyn_cast<llvm::DICompositeType>(try_next_di.value());
      LOG_DEBUG("  >> Try to recurse to " << log::ditype_str(next_ditype))
      assert(next_ditype && "We expect a composite type here");
    }
  } while (!endpoint_reached);

  return next_ditype;
}

}  // namespace dimeta::tbaa
