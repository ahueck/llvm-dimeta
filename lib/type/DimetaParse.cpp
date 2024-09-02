//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DimetaParse.h"

#include "DIParser.h"
#include "DIVisitor.h"
#include "DimetaData.h"
#include "support/Logger.h"

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
                                  std::string_view typedef_name = "", bool is_vector = false,
                                  bool is_foward_decl = false, bool recurring = false) {
  static_assert(std::is_same_v<T, CompoundType> || std::is_same_v<T, FundamentalType>, "Wrong type.");
  return QualType<T>{std::forward<T>(type), size, qual, typedef_name.data(), is_vector, is_foward_decl, recurring};
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
    case DW_ATE_UTF:
      return FundamentalType::Encoding::kUTFChar;
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
inline ArraySize make_array_size(const Type& type, Extent array_size_in_bits, const diparser::state::MetaData& meta_) {
  const auto array_byte_size = (array_size_in_bits / 8);
  if (meta_.array_of_pointer > 0) {
    return array_byte_size / (meta_.array_of_pointer / 8);
  } else if (type.extent > 0) {
    return array_byte_size / type.extent;
  }
  return 0;
}

QualifiedFundamental make_qualified_fundamental(const diparser::state::MetaData& meta_, std::string_view name,
                                                FundamentalType::Encoding encoding) {
  const auto size        = (meta_.type->getSizeInBits() / 8);
  auto fundamental       = FundamentalType{std::string{name}, size, encoding};
  const Qualifiers quals = helper::make_qualifiers(meta_.dwarf_tags);
  const auto array_size  = helper::make_array_size(fundamental, meta_.array_size_bits, meta_);

  auto qual_type_fundamental = helper::make_qual_type(std::move(fundamental), array_size, quals, meta_.typedef_name,
                                                      meta_.is_vector, meta_.is_forward_decl, false);
  return qual_type_fundamental;
}

}  // namespace helper

class DITypeParser final : public diparser::DIParseEvents {
  using CompoundStack = llvm::SmallVector<QualifiedCompound, 4>;
  using EnumBase      = std::optional<QualifiedFundamental>;
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

  template <typename QualType>
  void emplace_member(QualType&& type, const diparser::state::MetaData& meta_) {
    static_assert(std::is_same_v<QualType, QualifiedCompound> || std::is_same_v<QualType, QualifiedFundamental>,
                  "Wrong QualType for member.");
    assert(!composite_stack_.empty() && "Member requires composite on stack");
    auto& containing_composite = composite_stack_.back().type;
    containing_composite.offsets.emplace_back(meta_.member_offset);
    containing_composite.sizes.emplace_back(meta_.member_size);
    containing_composite.members.emplace_back(
        helper::make_member<QualType>(meta_.member_name, std::forward<QualType>(type)));
  }

  void emplace_fundamental(const diparser::state::MetaData& meta_, std::string_view name,
                           FundamentalType::Encoding encoding = FundamentalType::kUnknown) {
    auto qual_type_fundamental = helper::make_qualified_fundamental(meta_, name, encoding);

    if (meta_.is_member) {
      emplace_member(std::move(qual_type_fundamental), meta_);
      return;
    }

    // Workaround for composite:
    //    if (!composite_stack_.empty()) {
    //      const auto& composite = composite_stack_.back().type;
    //      if (composite.type == CompoundType::Tag::kEnum || composite.type == CompoundType::Tag::kEnumClass) {
    //        emplace_member(std::move(qual_type_fundamental), meta_);
    //        return;
    //      }
    //    }
    //
    emplace_result<QualifiedFundamental>(std::move(qual_type_fundamental));
  }

  void make_fundamental(const diparser::state::MetaData& meta_) override {
    const auto* basic_type = llvm::dyn_cast<llvm::DIBasicType>(meta_.type);
    assert(basic_type != nullptr && "DIBasicType should not be null at this point");
    const auto name = basic_type->getName();

    const auto encoding = (!name.empty() && name.contains("nullptr"))
                              ? FundamentalType::Encoding::kNullptr
                              : helper::dwarf2encoding(basic_type->getEncoding());

    emplace_fundamental(meta_, basic_type->getName(), encoding);
  }

  void make_void_ptr(const diparser::state::MetaData& meta_) override {
    const auto* derived_type = llvm::dyn_cast<llvm::DIDerivedType>(meta_.type);
    assert(derived_type != nullptr && "Type void* should be a derived type");
    emplace_fundamental(meta_, "void", FundamentalType::Encoding::kVoid);
  }

  void make_vtable(const diparser::state::MetaData& meta_) override {
    const auto* derived_type = llvm::dyn_cast<llvm::DIDerivedType>(meta_.type);
    assert(derived_type != nullptr && "Vtable should be a derived type");
    assert(meta_.is_member && "Vtable should be a member of composite");
    emplace_fundamental(meta_, derived_type->getName(), FundamentalType::Encoding::kVtablePtr);
  }

  void make_enum_member(const diparser::state::MetaData& meta_) override {
    const auto* basic_type = llvm::dyn_cast<llvm::DIBasicType>(meta_.type);
    assert(basic_type != nullptr && "DIBasicType should not be null for enum value");
    assert(!composite_stack_.empty() && "Requires a composite type on stack");
    assert((composite_stack_.back().type.type == CompoundType::Tag::kEnumClass ||
            composite_stack_.back().type.type == CompoundType::Tag::kEnum) &&
           "Requires a enum type on stack");

    emplace_fundamental(meta_, basic_type->getName(), helper::dwarf2encoding(basic_type->getEncoding()));

    auto& enum_type = composite_stack_.back().type;
    if (enum_type.sizes.size() > 1)
      enum_type.sizes.erase(std::next(std::begin(enum_type.sizes)), std::end(enum_type.sizes));
    if (enum_type.offsets.size() > 1)
      enum_type.offsets.erase(std::next(std::begin(enum_type.offsets)), std::end(enum_type.offsets));
  }

  void make_composite(const diparser::state::MetaData& meta_) override {
    auto* composite_type = llvm::dyn_cast<llvm::DICompositeType>(meta_.type);
    assert(composite_type != nullptr);

    auto compound_type =
        helper::make_compound(composite_type->getName(), composite_type->getIdentifier(),
                              static_cast<llvm::dwarf::Tag>(composite_type->getTag()), composite_type->getSizeInBits());
    const Qualifiers quals = helper::make_qualifiers(meta_.dwarf_tags);
    const auto array_size  = helper::make_array_size(compound_type, meta_.array_size_bits, meta_);

    const QualifiedCompound q_compound{compound_type,      array_size,      quals,
                                       meta_.typedef_name, meta_.is_vector, meta_.is_forward_decl,
                                       meta_.is_recurring};
    composite_stack_.emplace_back(std::move(q_compound));
  }

  void finalize_composite(const diparser::state::MetaData& current_meta) override {
    assert(!composite_stack_.empty() && "Requires a composite type on stack");
    auto finalized_composite = composite_stack_.pop_back_val();

    if (current_meta.is_member) {
      emplace_member(std::move(finalized_composite), current_meta);
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

    assert(composite_stack_.empty() && "Assumes top level compound here");
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
