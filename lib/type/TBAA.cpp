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
#include <iterator>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/SmallVector.h>
#include <optional>
#include <string>
#include <string_view>

namespace dimeta::tbaa {

struct TBAAHandle {
  llvm::MDNode* base_ty;
  llvm::MDNode* access_ty;
  llvm::ConstantInt* offset;
  size_t offset_value{};

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

  void reset_base(llvm::MDNode* new_base_ty) {
    LOG_DEBUG("Reset base_type " << log::ditype_str(base_ty) << " to " << log::ditype_str(new_base_ty))
    base_ty = new_base_ty;
  }

  void subtract_offset(size_t offset_) {
    LOG_DEBUG("Reset offset value " << offset_value << " with subtraction of " << offset_)
    assert(offset_ <= offset_value && "New offset should not be greater than current one");
    offset_value -= offset_;
  }

 private:
  explicit TBAAHandle(llvm::MDNode& tbaa_node) {
    base_ty       = llvm::dyn_cast<llvm::MDNode>(tbaa_node.getOperand(0));
    access_ty     = llvm::dyn_cast<llvm::MDNode>(tbaa_node.getOperand(1));
    auto value_md = llvm::dyn_cast<llvm::ValueAsMetadata>(tbaa_node.getOperand(2))->getValue();
    assert(value_md && "Offset value may not be null.");
    offset       = llvm::dyn_cast<llvm::ConstantInt>(value_md);
    offset_value = offset->getLimitedValue();
  }
};

namespace helper {

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
  std::optional<llvm::DICompositeType*> result{};
  explicit FindMatchingMember(llvm::StringRef composite_name) : composite_name_(composite_name) {
  }

  bool visitCompositeType(const llvm::DICompositeType* type) {
    const auto id = [](const auto* type) -> llvm::StringRef {
      if (type->getIdentifier().empty()) {
        return type->getName();
      }
      return type->getIdentifier();
    }(type);

    if (id == composite_name_) {
      result = const_cast<llvm::DICompositeType*>(type);
      return false;
    }

    return true;
  }
};

std::optional<llvm::DICompositeType*> find_composite_with_name(llvm::StringRef composite_name,
                                                               const llvm::DICompositeType* type) {
  FindMatchingMember finder{composite_name};
  finder.traverseCompositeType(type);
  return finder.result;
}

inline size_t tbaa_operand_count(llvm::MDNode* type_node) {
  // Given, e.g., {!"_ZTSNSt12_Vector_baseIPiSaIS0_EE17_Vector_impl_dataE", !749, i64 0, !749, i64 8, !749, i64 16}
  // returns number of [type, offsets] pairs: 3
  const auto num = type_node->getNumOperands();
  assert(num > 0 && "Operand count must be > 0");
  return (num - 1) / 2;
}

inline std::optional<llvm::StringRef> tbaa_operand_name(llvm::MDNode* type_node) {
  if (type_node->getNumOperands() == 0) {
    return {};
  }
  return llvm::dyn_cast<llvm::MDString>(type_node->getOperand(0))->getString();
}

struct TBAADestructure {
  llvm::StringRef name;
  size_t offset_base_{};
  std::optional<llvm::MDNode*> outermost_candidate_{};

  bool traverse_tbaa_nodes(llvm::MDNode* new_base_ty) {
    const auto tbaa_name = tbaa_operand_name(new_base_ty).value_or("");
    if (!tbaa_name.empty() && name == tbaa_name) {
      LOG_DEBUG("Match found " << log::ditype_str(new_base_ty))
      outermost_candidate_.emplace(new_base_ty);
      return true;
    }

    const auto* end_node = std::prev(new_base_ty->op_end());
    for (const auto* iter = new_base_ty->op_begin(); iter != end_node; ++iter) {
      const auto& operand = *iter;

      if (auto* mdnode = llvm::dyn_cast<llvm::MDNode>(operand)) {
        auto result = traverse_tbaa_nodes(mdnode);

        if (result) {
          // we found a matching node, we need to set the offset now to re-calculate later
          const auto* offset = std::next(iter);
          if (auto* value_md = llvm::dyn_cast<llvm::ValueAsMetadata>(*offset)) {
            if (auto* current_offset = llvm::dyn_cast<llvm::ConstantInt>(value_md->getValue())) {
              this->offset_base_ += current_offset->getLimitedValue();
            }
          }
          break;
        }
      }
    }

    return false;
  }
};

inline std::pair<std::optional<llvm::MDNode*>, size_t> tbaa_sub_node_matches_name(llvm::StringRef name,
                                                                                  llvm::MDNode* new_base_ty) {
  TBAADestructure tbaa{name};
  const auto found = tbaa.traverse_tbaa_nodes(new_base_ty);
  LOG_DEBUG("Found " << log::ditype_str(tbaa.outermost_candidate_.value_or(nullptr)) << " at offset "
                     << tbaa.offset_base_)
  return {tbaa.outermost_candidate_, tbaa.offset_base_};
}

}  // namespace helper

bool composite_fits_tbaa(const llvm::DICompositeType* composite, const TBAAHandle& tbaa) {
  const auto num_members = helper::num_composite_members(composite);
  const auto tbaa_count  = helper::tbaa_operand_count(tbaa.base_ty);
  if (num_members != tbaa_count) {
    return false;
  }

  LOG_DEBUG("Type element size " << num_members << " vs. TBAA " << tbaa_count)

  const auto elements  = helper::composite_members(composite);
  int element_position = 0;  // Incremented for every TBAA constant int entry
  // Loop simply checks if the byte offsets are the same (TODO also compare types!)
  for (const auto& tbaa_operand : tbaa.base_ty->operands()) {
    if (auto* value_md = llvm::dyn_cast<llvm::ValueAsMetadata>(tbaa_operand)) {
      if (auto* current_offset = llvm::dyn_cast<llvm::ConstantInt>(value_md->getValue())) {
        auto* current_member = elements[element_position];
        if (!current_offset->equalsInt(current_member->getOffsetInBits() / 8)) {
          return false;
        }
        ++element_position;
      }
    }
  }

  return true;
}

std::optional<llvm::DIType*> tbaa_resolver(llvm::DIType* root, TBAAHandle& tbaa) {
  const auto find_composite = [](llvm::DIType* root) {
    llvm::DIType* type = root;
    while (type && llvm::isa<llvm::DIDerivedType>(type)) {
      auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
      type        = ditype->getBaseType();
    }
    return type;
  };

  auto* maybe_composite = find_composite(root);
  if (!maybe_composite || !llvm::isa<llvm::DICompositeType>(maybe_composite)) {
    LOG_DEBUG("Returning: DIType is not composite required for TBAA descend.")
    return root;
  }

  auto* composite = llvm::dyn_cast<llvm::DICompositeType>(maybe_composite);

  LOG_DEBUG("Found maybe legible composite node: " << log::ditype_str(composite))

  // Cpp TBAA uses identifier, in C we use the name:
  const auto struct_name = [](const auto* composite) {
    auto name = std::string{composite->getIdentifier()};
    if (name.empty()) {
      name = std::string{composite->getName()};
    }
    return name;
  }(composite);

  if (struct_name != tbaa.base_name() && !tbaa.base_name().empty()) {
    LOG_DEBUG("Names differ. Name of struct: \"" << struct_name << "\" vs name of TBAA \"" << tbaa.base_name() << "\".")
    auto result_composite = helper::find_composite_with_name(tbaa.base_name(), composite);

    if (result_composite) {
      LOG_DEBUG("Found matching sub member " << log::ditype_str(result_composite.value()))
      composite = result_composite.value();  // const_cast<llvm::DICompositeType*>(result_composite.value());
      const auto num_members = helper::num_composite_members(composite);
      const auto num_tbaa    = helper::tbaa_operand_count(tbaa.base_ty);
      if (num_members != num_tbaa) {
        LOG_DEBUG("Mismatch between sub member element count and TBAA base type count "
                  << log::ditype_str(tbaa.base_ty))
        return root;
      }
    } else {
      LOG_DEBUG("Did not find matching sub member: " << struct_name)
      const auto [new_base, calculated_offset] = helper::tbaa_sub_node_matches_name(struct_name, tbaa.base_ty);
      if (!new_base) {
        LOG_DEBUG("Did not find new base, returning")
        return root;
      }
      LOG_DEBUG("New TBAA base " << log::ditype_str(new_base.value()) << " with negative offset " << calculated_offset)
      tbaa.reset_base(new_base.value());
      tbaa.subtract_offset(calculated_offset);
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
  LOG_DEBUG("  At offset: " << tbaa.offset_value)

  auto result = visitor::util::resolve_byte_offset_to_member_of(composite, tbaa.offset_value);
  if (result) {
    return result->type_of_member;
  }
  return {};
}

std::optional<llvm::DIType*> resolve_tbaa(llvm::DIType* root, const llvm::Instruction& instruction) {
  LOG_DEBUG("Resolve TBAA of instruction '" << instruction << "' with ditype: " << log::ditype_str(root))

  auto tbaa = TBAAHandle::create(instruction);
  if (!tbaa) {
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
