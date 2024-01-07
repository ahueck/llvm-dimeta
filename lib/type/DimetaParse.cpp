//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DimetaParse.h"

#include "DIParser.h"
#include "DIVisitor.h"

#include "llvm/ADT/STLExtras.h"

#include <type_traits>

namespace dimeta::parser {
namespace helper {

template <typename T, typename... Args>
inline std::shared_ptr<Member> make_member(std::string_view name, Args&&... args) {
  static_assert(std::is_same_v<T, QualifiedCompound> || std::is_same_v<T, QualifiedFundamental>, "Wrong qualtype.");
  return std::make_shared<Member>(Member{name.data(), T{std::forward<Args>(args)...}});
}

template <typename T>
inline std::shared_ptr<BaseClass> make_base(T&& compound) {  //, BaseClass::VTable&& vtable = {}) {
  static_assert(std::is_same_v<typename std::remove_cv<T>::type, QualifiedCompound>, "Need a qualified compound.");
  return std::make_shared<BaseClass>(
      BaseClass{std::forward<T>(compound)});  //, std::forward<BaseClass::VTable>(vtable)});
}

template <typename T>
inline QualType<T> make_qual_type(T&& type, ArraySize size = 0, Qualifiers qual = {Qualifier::kNone},
                                  std::string_view typedef_name = "", bool recurring = false) {
  static_assert(std::is_same_v<T, CompoundType> || std::is_same_v<T, FundamentalType>, "Wrong type.");
  return QualType<T>{std::forward<T>(type), size, qual, typedef_name.data(), recurring};
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

inline Qualifiers make_qualifiers(const llvm::SmallVector<unsigned, 8>& tag_collector) {
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

class DITypeParser final : public diparser::DIParseEvents {
  using CompoundStack = llvm::SmallVector<QualifiedCompound, 4>;
  DimetaParseResult result_;
  CompoundStack composite_stack_;

 public:
  [[nodiscard]] const DimetaParseResult& getParsedType() const {
    return result_;
  }

  template <typename QualType>
  void emplace_result(QualType&& type) {
    result_.type_.emplace<QualType>(std::forward<QualType>(type));
  }

  void make_void_ptr(const diparser::state::MetaData& meta_) override {
    const auto* derived_type = llvm::dyn_cast<llvm::DIDerivedType>(meta_.type);
    assert(derived_type != nullptr);

    const auto size        = meta_.is_member ? meta_.member_size : (derived_type->getSizeInBits() / 8);
    auto fundamental       = FundamentalType{std::string{"void*"}, size, FundamentalType::kUnknown};
    const Qualifiers quals = helper::make_qualifiers(meta_.dwarf_tags);
    const auto array_size  = helper::make_array_size(fundamental, meta_.array_size_bits);

    if (meta_.is_member) {
      assert(!composite_stack_.empty() && "Member requires composite on stack");
      auto& containing_composite = composite_stack_.back().type;
      containing_composite.offsets.emplace_back(meta_.member_offset);
      containing_composite.sizes.emplace_back(meta_.member_size);

      containing_composite.members.emplace_back(helper::make_member<QualifiedFundamental>(
          meta_.member_name, std::move(fundamental), array_size, Qualifiers{quals.back()}, meta_.typedef_name));
      return;
    }

    emplace_result<QualifiedFundamental>(
        helper::make_qual_type(std::move(fundamental), array_size, quals, meta_.typedef_name, false));
  }

  void make_fundamental(const diparser::state::MetaData& meta_) override {
    const auto* basic_type = llvm::dyn_cast<llvm::DIBasicType>(meta_.type);
    assert(basic_type != nullptr && "DIBasicType should not be null at this point");

    auto fundamental       = FundamentalType{std::string{basic_type->getName()}, (basic_type->getSizeInBits() / 8),
                                       helper::dwarf2encoding(basic_type->getEncoding())};
    const Qualifiers quals = helper::make_qualifiers(meta_.dwarf_tags);
    const auto array_size  = helper::make_array_size(fundamental, meta_.array_size_bits);

    if (meta_.is_member) {
      assert(!composite_stack_.empty() && "Fundamental member requires member on stack");
      auto& containing_composite = composite_stack_.back().type;
      containing_composite.offsets.emplace_back(meta_.member_offset);
      containing_composite.sizes.emplace_back(meta_.member_size);
      containing_composite.members.emplace_back(helper::make_member<QualifiedFundamental>(
          meta_.member_name, std::move(fundamental), array_size, quals, meta_.typedef_name, false));
      return;
    }

    emplace_result<QualifiedFundamental>(
        helper::make_qual_type(std::move(fundamental), array_size, quals, meta_.typedef_name, false));
  }

  void make_vtable(const diparser::state::MetaData& meta_) override {
    assert(!composite_stack_.empty() && "Vtable requires composite on stack");
    const auto* derived_type = llvm::dyn_cast<llvm::DIDerivedType>(meta_.type);
    assert(derived_type != nullptr);

    auto& containing_composite = composite_stack_.back().type;
    containing_composite.offsets.emplace_back(meta_.member_offset);
    containing_composite.sizes.emplace_back(meta_.member_size);

    auto fundamental =
        FundamentalType{std::string{derived_type->getName()}, (meta_.member_size), FundamentalType::kVtablePtr};

    const Qualifiers quals = helper::make_qualifiers(meta_.dwarf_tags);
    const auto array_size  = helper::make_array_size(fundamental, meta_.array_size_bits);

    containing_composite.members.emplace_back(helper::make_member<QualifiedFundamental>(
        meta_.member_name, std::move(fundamental), array_size, Qualifiers{quals.back()}, meta_.typedef_name));
  }

  void make_composite(const diparser::state::MetaData& meta_) override {
    auto* composite_type = llvm::dyn_cast<llvm::DICompositeType>(meta_.type);
    assert(composite_type != nullptr);

    auto compound_type =
        helper::make_compound(composite_type->getName(), composite_type->getIdentifier(),
                              static_cast<llvm::dwarf::Tag>(composite_type->getTag()), composite_type->getSizeInBits());
    const Qualifiers quals = helper::make_qualifiers(meta_.dwarf_tags);
    const auto array_size  = helper::make_array_size(compound_type, meta_.array_size_bits);

    const QualifiedCompound q_compound{compound_type, array_size, quals, meta_.typedef_name, meta_.is_recurring};
    composite_stack_.emplace_back(std::move(q_compound));
  }

  void finalize_composite(const diparser::state::MetaData& current_meta) override {
    assert(!composite_stack_.empty() && "Requires a composite type on stack");
    auto finalized_composite = composite_stack_.pop_back_val();

    if (current_meta.is_member) {
      auto& containing_composite = composite_stack_.back().type;
      containing_composite.offsets.emplace_back(current_meta.member_offset);
      containing_composite.sizes.emplace_back(current_meta.member_size);
      containing_composite.members.emplace_back(
          helper::make_member<QualifiedCompound>(current_meta.member_name, std::move(finalized_composite)));
      return;
    }

    if (current_meta.is_base_class) {
      const auto base            = helper::make_base(std::move(finalized_composite));
      auto& containing_composite = composite_stack_.back().type;
      containing_composite.offsets.emplace_back(current_meta.member_offset);
      containing_composite.sizes.emplace_back(finalized_composite.type.extent);
      containing_composite.bases.emplace_back(std::move(base));
      return;
    }

    assert(composite_stack_.empty());
    emplace_result<QualifiedCompound>(std::move(finalized_composite));
  }
};

std::optional<DimetaParseResult> make_dimetadata(const llvm::DINode* node) {
  if (!(llvm::isa<llvm::DIVariable>(node) || llvm::isa<llvm::DIType>(node))) {
    return {};
  }
  DITypeParser parser;
  diparser::visit_node(node, parser);
  return parser.getParsedType();
}

}  // namespace dimeta::parser