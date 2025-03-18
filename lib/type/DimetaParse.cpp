//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
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

#include <iterator>
#include <llvm/BinaryFormat/Dwarf.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <numeric>  // for std::accumulate
#include <tuple>
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
    case DW_ATE_complex_float:
      return FundamentalType::Encoding::kComplex;
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
    case DW_TAG_ptr_to_member_type:
      return Qualifier::kPtrToMember;
    case DW_TAG_array_type:
      return Qualifier::kArray;
    case diparser::state::CustomDwarfTag::kVector:
      return Qualifier::kVector;
    default:
      return Qualifier::kNone;
  }
};

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
inline ArraySizeList make_array_sizes(const Type&,
                                      const std::vector<diparser::state::MetaData::ArrayData>& meta_array_data) {
  ArraySizeList list;
  if (meta_array_data.empty()) {
    return list;
  }
  // const auto array_size_calc = [&type](const diparser::state::MetaData::ArrayData& array, bool is_last) {
  //   const auto array_byte_size = (array.array_size_bits / 8);
  //   // is an array of pointers:
  //   if (array.array_of_pointer > 0) {
  //     return array_byte_size / (array.array_of_pointer / 8);
  //   }
  //   // is an array of the "type":
  //   if (is_last && type.extent > 0) {
  //     return array_byte_size / type.extent;
  //   }
  //   return array_byte_size;
  // };
  const auto array_size_calc_sub = [](const diparser::state::MetaData::ArrayData& array, bool) {
    // LOG_FATAL(array.subranges.size());
    ArraySize sum =
        std::accumulate(array.subranges.begin(), array.subranges.end(), ArraySize{1}, std::multiplies<ArraySize>());
    return sum;
  };

  for (auto it = meta_array_data.begin(); it != meta_array_data.end(); ++it) {
    const auto& array          = *it;
    const bool is_last_element = (it == std::prev(std::end(meta_array_data)));
    const auto size            = array_size_calc_sub(array, is_last_element);
    list.emplace_back(size);
    // LOG_FATAL("Array: " << size);
  }

  return list;
}

template <typename T>
inline QualType<T> make_qual_type(const T& type, const diparser::state::MetaData& meta_) {
  static_assert(std::is_same_v<T, CompoundType> || std::is_same_v<T, FundamentalType>, "Wrong type.");

  Qualifiers quals = helper::make_qualifiers(meta_.dwarf_tags);
  if (meta_.is_member_static) {
    // TODO should this be the last, or should it be position dependent w.r.t. dwarf_tags?
    quals.emplace_back(Qualifier::kStatic);
  }
  const auto array_size   = helper::make_array_sizes(type, meta_.arrays);
  const auto typedef_name = meta_.typedef_names.empty() ? std::string{} : *meta_.typedef_names.begin();

  Extent vec_size{0};
  for (const auto& array_data : meta_.arrays) {
    if (array_data.is_vector) {
      assert(vec_size == 0 && "Multiple vectors detected in arrays.");
      vec_size = array_data.array_size_bits / 8;
    }
  }

  return QualType<T>{
      type, array_size, quals, typedef_name, vec_size, meta_.is_vector, meta_.is_forward_decl, meta_.is_recurring};
}

inline CompoundType make_compound(const llvm::DICompositeType* composite_type) {
  auto compound =
      [](llvm::StringRef compound_name, llvm::StringRef compound_identifier, llvm::dwarf::Tag tag,
         Extent size_in_bits) {
        return CompoundType{std::string{compound_name}, std::string{compound_identifier}, dwarf2compound(tag),
                            size_in_bits / 8};
      }(composite_type->getName(), composite_type->getIdentifier(),
        static_cast<llvm::dwarf::Tag>(composite_type->getTag()), composite_type->getSizeInBits());
  return compound;
}

inline QualifiedCompound make_qualified_compound(const diparser::state::MetaData& meta_) {
  return make_qual_type<CompoundType>(make_compound(llvm::dyn_cast<llvm::DICompositeType>(meta_.type)), meta_);
}

QualifiedFundamental make_qualified_fundamental(const diparser::state::MetaData& meta_, std::string_view name,
                                                FundamentalType::Encoding encoding) {
  const auto size = [&]() {
    auto size = (meta_.type->getSizeInBits() / 8);
    if (size == 0) {
      if (encoding == FundamentalType::Encoding::kNullptr) {
        // sizeof std::nullptr == sizeof void*
        size = meta_.member_size > 0 ? meta_.member_size : sizeof(void*);
      }
    }
    return size;
  }();
  auto fundamental = FundamentalType{std::string{name}, size, encoding};
  return make_qual_type<FundamentalType>(fundamental, meta_);
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

  template <typename QualType>
  void emplace_member(QualType&& type, const diparser::state::MetaData& meta_) {
    static_assert(std::is_same_v<QualType, QualifiedCompound> || std::is_same_v<QualType, QualifiedFundamental>,
                  "Wrong QualType for member.");
    assert(!composite_stack_.empty() && "Member requires composite on stack");
    auto& containing_composite = composite_stack_.back().type;
    if (meta_.is_member_static) {
      containing_composite.static_members.emplace_back(
          helper::make_member<QualType>(meta_.member_name, std::forward<QualType>(type)));
    } else {
      containing_composite.offsets.emplace_back(meta_.member_offset);
      containing_composite.sizes.emplace_back(meta_.member_size);
      containing_composite.members.emplace_back(
          helper::make_member<QualType>(meta_.member_name, std::forward<QualType>(type)));
    }
  }

  void emplace_fundamental(const diparser::state::MetaData& meta_, std::string_view name,
                           FundamentalType::Encoding encoding = FundamentalType::kUnknown) {
    auto qual_type_fundamental = helper::make_qualified_fundamental(meta_, name, encoding);

    if (meta_.is_member) {
      emplace_member(std::move(qual_type_fundamental), meta_);
      return;
    }

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
    if (enum_type.sizes.size() > 1) {
      // emplace_fundamental adds enum value as "member" with offset in enum compound, but we want it to be "1" member
      // only:
      enum_type.sizes.erase(std::next(std::begin(enum_type.sizes)), std::end(enum_type.sizes));
    }
    if (enum_type.offsets.size() > 1) {
      // emplace_fundamental adds enum value as "member" with offset in enum compound, but we want it to be "1" member
      // only:
      enum_type.offsets.erase(std::next(std::begin(enum_type.offsets)), std::end(enum_type.offsets));
    }
  }

  void make_composite(const diparser::state::MetaData& meta_) override {
    assert(llvm::dyn_cast<llvm::DICompositeType>(meta_.type) != nullptr);

    const QualifiedCompound q_compound = helper::make_qualified_compound(meta_);
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
      const auto base          = helper::make_base(std::move(finalized_composite));
      const bool size_one      = base->base.type.extent == 1;
      const bool empty_members = base->base.type.members.empty();
      // const bool ebo_base        = base->base.type.bases.size() > 1 &&
      // base->base.type.bases.front()->empty_base_class;
      base->is_empty_base_class  = size_one && empty_members;
      base->offset               = current_meta.member_offset;
      auto& containing_composite = composite_stack_.back().type;
      // if (!base->is_empty_base_class) {
      //   containing_composite.offsets.emplace_back(current_meta.member_offset);
      //   containing_composite.sizes.emplace_back(finalized_composite.type.extent);
      // }
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
