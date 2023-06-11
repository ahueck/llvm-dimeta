//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_METAPARSE_H
#define DIMETA_METAPARSE_H

#include "DIVisitor.h"
#include "DimetaData.h"
#include "Util.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace dimeta::parser {

namespace helper {

template <typename T, typename... Args>
inline std::shared_ptr<Member> make_member(std::string_view name, Args&&... args) {
  static_assert(std::is_same_v<T, QualifiedCompound> || std::is_same_v<T, QualifiedFundamental>, "Wrong qualtype.");
  return std::make_shared<Member>(Member{name.data(), T{std::forward<Args>(args)...}});
}

template <typename T>
inline std::shared_ptr<BaseClass> make_base(T&& compound, BaseClass::VTable&& vtable = {}) {
  static_assert(std::is_same_v<typename std::remove_cv<T>::type, QualifiedCompound>, "Need a qualified compound.");
  return std::make_shared<BaseClass>(BaseClass{std::forward<T>(compound), std::forward<BaseClass::VTable>(vtable)});
}

template <typename T>
inline QualType<T> make_qual_type(T&& type, ArraySize size = 0, Qualifiers qual = {Qualifier::kNone},
                                  std::string_view typedef_name = "") {
  static_assert(std::is_same_v<T, CompoundType> || std::is_same_v<T, FundamentalType>, "Wrong type.");
  return QualType<T>{std::forward<T>(type), size, qual, typedef_name.data()};
}

inline CompoundType::Tag dwarf2compound(const llvm::dwarf::Tag tag) {
  using namespace llvm::dwarf;
  switch (tag) {
    case DW_TAG_enumeration_type:
      return dimeta::CompoundType::Tag::kEnum;
    case DW_TAG_class_type:
      return dimeta::CompoundType::Tag::kClass;
    case DW_TAG_structure_type:
      return dimeta::CompoundType::Tag::kStruct;
    case DW_TAG_union_type:
      return dimeta::CompoundType::Tag::kUnion;
    default:
      return dimeta::CompoundType::Tag::kUnknown;
  }
}

inline FundamentalType::Encoding dwarf2encoding(const unsigned di_encoding) {
  using namespace llvm::dwarf;
  switch (di_encoding) {
    case DW_ATE_float:
      return FundamentalType::Encoding::kFloat;
    case DW_ATE_signed:
      return FundamentalType::Encoding::kSignedInt;
    case DW_ATE_unsigned:
      return FundamentalType::Encoding::kUnsignedInt;
    case DW_ATE_unsigned_char:
      return FundamentalType::Encoding::kUnsignedChar;
    case DW_ATE_signed_char:
      return FundamentalType::Encoding::kSignedChar;
    case DW_ATE_boolean:
      return FundamentalType::Encoding::kBool;
    default:
      return FundamentalType::Encoding::kUnknown;
  }
}

inline Qualifier dwarf2qual(unsigned tag) {
  using namespace llvm::dwarf;
  switch (tag) {
    case DW_TAG_pointer_type:
      return Qualifier::kPtr;
    case DW_TAG_reference_type:
      return Qualifier::kRef;
    case DW_TAG_const_type:
      return Qualifier::kConst;
    default:
      return Qualifier::kNone;
  }
};

inline CompoundType make_compound(llvm::StringRef compound_name, llvm::StringRef compound_identifier,
                                  llvm::dwarf::Tag tag, Extent size_in_bits) {
  return CompoundType{std::string{compound_name}, std::string{compound_identifier}, dwarf2compound(tag),
                      size_in_bits / 8};
}

inline Qualifiers make_qual(const llvm::SmallVector<unsigned, 8>& tag_collector) {
  llvm::SmallVector<unsigned, 8> dwarf_quals;
  llvm::copy_if(tag_collector, std::back_inserter(dwarf_quals), [&](const auto& tag) {
    const auto tag_qual = helper::dwarf2qual(tag);
    return tag_qual != Qualifier::kNone;
  });

  Qualifiers quals;
  quals.reserve(tag_collector.size());
  llvm::transform(dwarf_quals, std::back_inserter(quals), [&](const auto& tag) -> Qualifier {
    const auto tag_qual = helper::dwarf2qual(tag);
    return Qualifier{tag_qual};
  });

  return quals;
}

template <typename Type>
inline ArraySize make_array_size(const Type& type, Extent array_size_in_bits) {
  const auto array_byte_size = (array_size_in_bits / 8);
  if (type.extent > 0) {
    return array_byte_size / type.extent;
  }
  return 0;
}

}  // namespace helper

class DITypeParser : public visitor::DINodeVisitor<DITypeParser> {
 private:
  std::variant<std::monostate, QualifiedCompound, QualifiedFundamental> result_;

  struct Meta {
    llvm::SmallVector<unsigned, 8> tag_collector;
    Extent member_offset{0};
    Extent member_size{0};
    Extent array_size_bits{0};
    Extent vtable_size{0};
    bool is_member{false};
    std::string member_name;
    std::string typedef_name;
    bool inherited{false};
    bool has_vtable{false};
    void clear() {
      tag_collector.clear();
      array_size_bits = 0;
      member_offset   = 0;
      vtable_size     = 0;
      is_member       = false;
      typedef_name    = "";
      member_name     = "";
      inherited       = false;
      has_vtable      = false;
    }
  } meta_{};

  struct MetaNode {
    QualifiedCompound q_compound;
    Meta data;
  };

  using CompositeStack = llvm::SmallVector<MetaNode, 4>;

  CompositeStack composite_stack_;

 public:
  [[nodiscard]] bool hasCompound() const {
    return std::holds_alternative<QualifiedCompound>(result_);
  }

  [[nodiscard]] bool hasFundamental() const {
    return std::holds_alternative<QualifiedFundamental>(result_);
  }

  template <typename QualifiedType>
  std::optional<QualifiedType> getAs() const {
    if (std::holds_alternative<QualifiedType>(result_)) {
      return std::get<QualifiedType>(result_);
    }
    return {};
  }

  bool visitBasicType(const llvm::DIBasicType* basic_type) {
    auto fundamental       = FundamentalType{std::string{basic_type->getName()}, (basic_type->getSizeInBits() / 8),
                                       helper::dwarf2encoding(basic_type->getEncoding())};
    const Qualifiers quals = helper::make_qual(meta_.tag_collector);
    const auto array_size  = helper::make_array_size(fundamental, meta_.array_size_bits);

    if (meta_.is_member) {
      assert(!composite_stack_.empty());
      auto& containing_composite = composite_stack_.back().q_compound.type;
      containing_composite.offsets.emplace_back(meta_.member_offset);
      containing_composite.sizes.emplace_back(meta_.member_size);
      containing_composite.members.emplace_back(helper::make_member<QualifiedFundamental>(
          meta_.member_name, std::move(fundamental), array_size, quals, meta_.typedef_name));

      return true;
    }

    result_.emplace<QualifiedFundamental>(
        helper::make_qual_type(std::move(fundamental), array_size, quals, meta_.typedef_name));

    return true;
  }

  bool visitDerivedType(const llvm::DIDerivedType* derived_type) {
    using namespace llvm::dwarf;

    const auto tag = derived_type->getTag();
    switch (tag) {
        //      case DW_TAG_array_type:
        //        meta_.array_size_bits = derived_type->getSizeInBits();
        //        break;
      case DW_TAG_member: {
        meta_.member_name   = derived_type->getName();
        meta_.is_member     = true;
        meta_.member_offset = derived_type->getOffsetInBits() / 8;
        meta_.member_size   = derived_type->getSizeInBits() / 8;
        //        const bool has_vtable = static_cast<bool>(derived_type->getFlags() &
        //        llvm::DINode::DIFlags::FlagArtificial); if (has_vtable) {
        //          assert(!composite_stack_.empty());
        //          composite_stack_.back().data.has_vtable = true;
        //        }
      } break;
      case DW_TAG_typedef:
        meta_.typedef_name = derived_type->getName();
        break;
      case DW_TAG_inheritance:
        meta_.inherited = true;
        break;
      default:
        meta_.tag_collector.emplace_back(tag);
    }

    // FIXME: hack so vtable pointer -> pointer is not applied to next type.
    if (derived_type->getName() == "__vtbl_ptr_type") {
      assert(!composite_stack_.empty());
      composite_stack_.back().data.vtable_size = derived_type->getSizeInBits() / 8;
      meta_.clear();
    }

    return true;
  }

  bool visitCompositeType(const llvm::DICompositeType* composite_type) {
    // See, e.g., pass/c/stack_struct_array.c:
    if (composite_type->getTag() == llvm::dwarf::DW_TAG_array_type) {
      meta_.array_size_bits = composite_type->getSizeInBits();
      return true;
    }

    const auto exit_clear = util::create_scope_exit([&]() { meta_.clear(); });
    auto compound_type =
        helper::make_compound(composite_type->getName(), composite_type->getIdentifier(),
                              static_cast<llvm::dwarf::Tag>(composite_type->getTag()), composite_type->getSizeInBits());
    const Qualifiers quals = helper::make_qual(meta_.tag_collector);
    const auto array_size  = helper::make_array_size(compound_type, meta_.array_size_bits);

    const QualifiedCompound q_compound{compound_type, array_size, quals, meta_.typedef_name};
    meta_.has_vtable = composite_type->getVTableHolder() == composite_type;
    if (meta_.has_vtable)
      llvm::outs() << *composite_type << "\n";
    composite_stack_.emplace_back(MetaNode{std::move(q_compound), meta_});

    return true;
  }

  void leaveCompositeType(const llvm::DICompositeType* t) {
    // See, e.g., pass/c/stack_struct_array.c:
    if (t->getTag() == llvm::dwarf::DW_TAG_array_type) {
      return;
    }

    const auto current_composite = composite_stack_.pop_back_val();
    const auto& current_meta     = current_composite.data;
    if (current_meta.is_member) {
      auto& containing_composite = composite_stack_.back().q_compound.type;
      containing_composite.offsets.emplace_back(current_meta.member_offset);
      containing_composite.sizes.emplace_back(current_meta.member_size);
      containing_composite.members.emplace_back(
          helper::make_member<QualifiedCompound>(current_meta.member_name, std::move(current_composite.q_compound)));
      return;
    }
    if (current_meta.inherited) {
      const auto base = current_meta.has_vtable ? helper::make_base(std::move(current_composite.q_compound),
                                                                    BaseClass::VTable{current_meta.vtable_size})
                                                : helper::make_base(std::move(current_composite.q_compound));
      composite_stack_.back().q_compound.type.bases.emplace_back(std::move(base));
      return;
    }
    assert(composite_stack_.empty());
    result_.emplace<QualifiedCompound>(std::move(current_composite.q_compound));
  }

  void leaveBasicType(const llvm::DIBasicType*) {
    meta_.clear();
  }
};

}  // namespace dimeta::parser
#endif  // DIMETA_METAPARSE_H
