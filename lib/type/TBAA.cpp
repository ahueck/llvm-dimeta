//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "TBAA.h"

#include "DIVisitor.h"
#include "DIVisitorUtil.h"
#include "DefUseAnalysis.h"
#include "support/Logger.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

#include <assert.h>
#include <cstddef>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/SmallVector.h>
#include <optional>
#include <string>
#include <string_view>

namespace dimeta::tbaa {

inline size_t num_composite_members(const llvm::DICompositeType* composite) {
  const auto num_members = llvm::count_if(composite->getElements(), [](const auto* node) {
    if (auto derived = llvm::dyn_cast<llvm::DIDerivedType>(node)) {
      return derived->getTag() == llvm::dwarf::DW_TAG_member;
    }
    return false;
  });
  return num_members;
}

inline llvm::SmallVector<llvm::DIDerivedType*, 4> composite_members(const llvm::DICompositeType* composite) {
  llvm::SmallVector<llvm::DIDerivedType*, 4> members;
  for (auto* member : composite->getElements()) {
    if (auto* derived = llvm::dyn_cast<llvm::DIDerivedType>(member)) {
      if (derived->getTag() == llvm::dwarf::DW_TAG_member) {
        members.push_back(derived);
      }
    }
  }

  return members;
}

class FindMatchingMember : public visitor::DINodeVisitor<FindMatchingMember> {
  llvm::StringRef composite_name_;

 public:
  std::optional<const llvm::DICompositeType*> result{};
  explicit FindMatchingMember(llvm::StringRef composite_name) : composite_name_(composite_name) {
  }
  // ~FindMatchingMember() = default;

  bool visitCompositeType(const llvm::DICompositeType* type) {
    const auto id = [](const auto* type) -> llvm::StringRef {
      if (type->getIdentifier().empty()) {
        return type->getName();
      }
      return type->getIdentifier();
    }(type);
    if (id == composite_name_) {
      result = type;
      return false;
    }
    return true;
  }
};

struct TBAAHandle {
  llvm::MDNode* base_ty;
  llvm::MDNode* access_ty;
  llvm::ConstantInt* offset;

  static std::optional<TBAAHandle> create(const llvm::Instruction& store) {
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

bool composite_fits_tbaa(const llvm::DICompositeType* composite, const TBAAHandle& tbaa) {
  const auto num_members_tbaa_node = [](llvm::MDNode* node) {
    const auto num = node->getNumOperands();
    assert(num > 0 && "Operand count must be > 0");
    return (num - 1) / 2;
  };

  const auto num_members = num_composite_members(composite);

  if (num_members != num_members_tbaa_node(tbaa.base_ty)) {
    return false;
  }

  LOG_DEBUG("Type element size " << num_members << " vs. TBAA " << num_members_tbaa_node(tbaa.base_ty))

  auto elements        = composite_members(composite);
  int element_position = 0;  // Incremented for every TBAA constant int entry
  // Loop simply checks if the byte offsets are the same (TODO also compare types!)
  for (auto& tbaa_operand : tbaa.base_ty->operands()) {
    if (auto value_md = llvm::dyn_cast<llvm::ValueAsMetadata>(tbaa_operand)) {
      if (auto current_offset = llvm::dyn_cast<llvm::ConstantInt>(value_md->getValue())) {
        auto current_member = elements[element_position];
        if (!current_offset->equalsInt(current_member->getOffsetInBits() / 8)) {
          return false;
        }
        ++element_position;
      }
    }
  }

  return true;
}

namespace iteration {
auto find_first_non_member_basetype(llvm::DIType* root) {
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

namespace detail {

inline bool is_pointer_like(const llvm::DIType& di_type) {
  if (const auto* type = llvm::dyn_cast<llvm::DIDerivedType>(&di_type)) {
    return type->getTag() == llvm::dwarf::DW_TAG_array_type || type->getTag() == llvm::dwarf::DW_TAG_reference_type ||
           type->getTag() == llvm::dwarf::DW_TAG_pointer_type ||
           type->getTag() == llvm::dwarf::DW_TAG_ptr_to_member_type;
  }
  return false;
}
}  // namespace detail

}  // namespace iteration

std::optional<llvm::DIType*> tbaa_resolver(llvm::DIType* root, const TBAAHandle& tbaa) {
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

  LOG_DEBUG("Found maybe legible composite node: " << log::ditype_str(composite))

  // Cpp TBAA uses identifier, in C we use the name:
  const auto struct_name = [](const auto* composite) {
    auto name = std::string{composite->getIdentifier()};
    if (name.empty()) {
      name = std::string{composite->getName()};
    }
    return name;
  }(composite);

  if (struct_name != tbaa.base_name()) {
    LOG_DEBUG("Names differ. Name of struct: \"" << struct_name << "\" vs name of TBAA \"" << tbaa.base_name() << "\".")
    FindMatchingMember finder{tbaa.base_name()};
    finder.traverseCompositeType(composite);

    if (finder.result) {
      LOG_DEBUG("Found matching sub member " << log::ditype_str(finder.result.value()))
      composite              = const_cast<llvm::DICompositeType*>(finder.result.value());
      const auto num_members = num_composite_members(composite);
      if (num_members != (tbaa.base_ty->getNumOperands() / 2)) {
        LOG_DEBUG("Mismatch between sub member element count and TBAA base type count "
                  << log::ditype_str(tbaa.base_ty))
        return root;
      }
    } else {
      LOG_DEBUG("Did not find matching sub member")
      return root;
    }
  }

  if (struct_name.empty()) {
    LOG_DEBUG("No name match possible")
    // Here no name matching is possible, is TBAA node referring to current DI type? We need to verify:
    if (!composite_fits_tbaa(composite, tbaa)) {
      LOG_DEBUG("TBAA of anonymous struct has different offsets to DIComposite type!")
      return root;
    }
  }

  assert(tbaa.access_is_ptr() && "TBAA access should be pointer");

  LOG_DEBUG("TBAA tree data.")
  LOG_DEBUG("  From ditype: " << log::ditype_str(composite))
  LOG_DEBUG("  From TBAA: " << log::ditype_str(tbaa.base_ty))
  LOG_DEBUG("  At offset: " << *tbaa.offset)

  auto result = visitor::util::resolve_byte_offset_to_member_of(composite, tbaa.offset->getLimitedValue());
  if (result) {
    return result->type_of_member;
  }
  return {};
}

std::optional<llvm::DIType*> resolve_tbaa(llvm::DIType* root, const llvm::Instruction& instruction) {
  LOG_DEBUG("Resolve TBAA of instruction '" << instruction << "' with ditype: " << log::ditype_str(root))

  auto tbaa = TBAAHandle::create(instruction);
  if (!tbaa.has_value()) {
    LOG_DEBUG("Requires TBAA metadata in LLVM IR.")
    return root;
  }
  // assign any ptr to any ptr, e.g., struct A** a; a[0] = malloc(struct A):
  if (tbaa->base_ty == tbaa->access_ty && tbaa->access_is_ptr()) {
    LOG_DEBUG("No work: TBAA base type is same as access type (both ptr).")
    return root;
  }
  const auto node = tbaa_resolver(root, tbaa.value());
  if (node.has_value() && node.value() != root) {
    LOG_DEBUG("Determined new type with TBAA")
  }
  return node;
}

}  // namespace dimeta::tbaa
