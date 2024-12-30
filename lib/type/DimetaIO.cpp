//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DimetaData.h"
#include "Util.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string_view>
#include <variant>

using namespace llvm::yaml;
using namespace llvm;
using llvm::yaml::IO;
using llvm::yaml::MappingTraits;
using llvm::yaml::ScalarEnumerationTraits;

using namespace dimeta;

template <typename Value>
inline void map_optional_not_empty(IO& io, std::string_view key, Value& data) {
  if (io.outputting()) {
    if constexpr (!std::is_integral_v<Value>) {
      if (!data.empty()) {
        io.mapOptional(key.data(), data);
      }
    } else {
      if (data != 0) {
        io.mapOptional(key.data(), data);
      }
    }
  } else {
    io.mapOptional(key.data(), data);
  }
}

template <>
struct llvm::yaml::SequenceTraits<Members> {
  static size_t size(IO& io, Members& list) {
    return list.size();
  }
  static Members::value_type& element(IO& io, Members& list, size_t index) {
    if (io.outputting()) {
      return list[index];
    }
    list.emplace_back(std::make_shared<Member>());
    return list.back();
  }
};

template <>
struct llvm::yaml::ScalarEnumerationTraits<dimeta::CompoundType::Tag> {
  static void enumeration(IO& io, dimeta::CompoundType::Tag& value) {
    using dimeta::CompoundType;
    io.enumCase(value, "class", dimeta::CompoundType::Tag::kClass);
    io.enumCase(value, "struct", dimeta::CompoundType::Tag::kStruct);
    io.enumCase(value, "union", dimeta::CompoundType::Tag::kUnion);
    io.enumCase(value, "enum", dimeta::CompoundType::Tag::kEnum);
    io.enumCase(value, "enum_class", dimeta::CompoundType::Tag::kEnumClass);
    io.enumCase(value, "unknown", dimeta::CompoundType::Tag::kUnknown);
  }
};

template <>
struct llvm::yaml::MappingTraits<std::shared_ptr<BaseClass>> {
  static void mapping(IO& io, std::shared_ptr<BaseClass>& info) {
    io.mapRequired("BaseClass", info->base);
    //    if (!io.outputting()) {
    //      Extent e{0};
    //      io.mapOptional("Vtable", e);
    //      if (e > 0) {
    //        info->vtable = BaseClass::VTable{e};
    //      }
    //      return;
    //    }
    //    if (info->vtable.has_value()) {
    //      io.mapOptional("Vtable", info->vtable.value());
    //    }
  }
};

template <>
struct llvm::yaml::SequenceTraits<Bases> {
  static size_t size(IO& io, Bases& list) {
    return list.size();
  }
  static Bases ::value_type& element(IO& io, Bases& list, size_t index) {
    if (io.outputting()) {
      return list[index];
    }
    list.emplace_back(std::make_shared<BaseClass>());
    return list.back();
  }
};

template <>
struct llvm::yaml::MappingTraits<CompoundType> {
  static void mapping(IO& io, CompoundType& info) {
    io.mapRequired("Name", info.name);
    //    io.mapOptional("Identifier", info.identifier);
    map_optional_not_empty(io, "Identifier", info.identifier);
    io.mapRequired("Type", info.type);
    io.mapRequired("Extent", info.extent);
    io.mapOptional("Sizes", info.sizes);
    io.mapOptional("Offsets", info.offsets);
    //    io.mapOptional("Base", info.bases);
    map_optional_not_empty(io, "Base", info.bases);
    io.mapOptional("Members", info.members);
  }
};

template <>
struct llvm::yaml::ScalarEnumerationTraits<dimeta::FundamentalType::Encoding> {
  static void enumeration(IO& io, dimeta::FundamentalType::Encoding& value) {
    using dimeta::CompoundType;
    io.enumCase(value, "unknown", dimeta::FundamentalType::Encoding::kUnknown);
    io.enumCase(value, "bool", dimeta::FundamentalType::Encoding::kBool);
    io.enumCase(value, "char", dimeta::FundamentalType::Encoding::kChar);
    io.enumCase(value, "float", dimeta::FundamentalType::Encoding::kFloat);
    io.enumCase(value, "int", dimeta::FundamentalType::Encoding::kInt);
    io.enumCase(value, "signed", dimeta::FundamentalType::Encoding::kSigned);
    io.enumCase(value, "unsigned", dimeta::FundamentalType::Encoding::kUnsigned);
    io.enumCase(value, "signed_char", dimeta::FundamentalType::Encoding::kSignedChar);
    io.enumCase(value, "unsigned_char", dimeta::FundamentalType::Encoding::kUnsignedChar);
    io.enumCase(value, "signed_int", dimeta::FundamentalType::Encoding::kSignedInt);
    io.enumCase(value, "unsigned_int", dimeta::FundamentalType::Encoding::kUnsignedInt);
    io.enumCase(value, "padding", dimeta::FundamentalType::Encoding::kPadding);
    io.enumCase(value, "void_ptr", dimeta::FundamentalType::Encoding::kVoid);
    io.enumCase(value, "vtable_ptr", dimeta::FundamentalType::Encoding::kVtablePtr);
    io.enumCase(value, "nullptr", dimeta::FundamentalType::Encoding::kNullptr);
    io.enumCase(value, "utf_char", dimeta::FundamentalType::Encoding::kUTFChar);
    io.enumCase(value, "complex", dimeta::FundamentalType::Encoding::kComplex);
  }
};

template <>
struct llvm::yaml::MappingTraits<FundamentalType> {
  static void mapping(IO& io, FundamentalType& info) {
    io.mapRequired("Name", info.name);
    io.mapOptional("Extent", info.extent);
    io.mapRequired("Encoding", info.encoding);
  }
  static const bool flow = true;
};

template <>
struct llvm::yaml::ScalarEnumerationTraits<dimeta::Qualifier> {
  static void enumeration(IO& io, dimeta::Qualifier& info) {
    io.enumCase(info, "none", dimeta::Qualifier::kNone);
    io.enumCase(info, "const", dimeta::Qualifier::kConst);
    io.enumCase(info, "ptr", dimeta::Qualifier::kPtr);
    io.enumCase(info, "ref", dimeta::Qualifier::kRef);
    io.enumCase(info, "ptr_to_mem", dimeta::Qualifier::kPtrToMember);
    io.enumCase(info, "array", dimeta::Qualifier::kArray);
    io.enumCase(info, "vector", dimeta::Qualifier::kVector);
  }
};

LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(dimeta::Qualifier)

template <typename QualType>
void map_qualtype_fields(IO& io, QualType&& info) {
  map_optional_not_empty(io, "Array", info.array_size);
  map_optional_not_empty(io, "Qualifiers", info.qual);
  map_optional_not_empty(io, "Typedef", info.typedef_name);
  map_optional_not_empty(io, "Vector", info.is_vector);
  map_optional_not_empty(io, "VectorSize", info.vector_size);
  map_optional_not_empty(io, "ForwardDecl", info.is_forward_decl);
  map_optional_not_empty(io, "Recurring", info.is_recurring);
}

template <>
struct llvm::yaml::MappingTraits<QualifiedFundamental> {
  static void mapping(IO& io, QualifiedFundamental& info) {
    io.mapRequired("Fundamental", info.type);
    map_qualtype_fields(io, info);
  }
};

template <>
struct llvm::yaml::MappingTraits<QualifiedCompound> {
  static void mapping(IO& io, QualifiedCompound& info) {
    io.mapRequired("Compound", info.type);
    map_qualtype_fields(io, info);
  }
};

inline void map_qualified_type(IO& io, QualifiedType& type) {
  bool holds_builtin{false};
  if (io.outputting()) {
    holds_builtin = bool(std::holds_alternative<QualifiedFundamental>(type));
  }
  io.mapRequired("Builtin", holds_builtin);

  if (holds_builtin) {
    if (!io.outputting()) {
      type.emplace<QualifiedFundamental>();
    }
    io.mapRequired("Type", std::get<QualifiedFundamental>(type));
    return;
  }

  if (!io.outputting()) {
    type.emplace<QualifiedCompound>();
  }
  io.mapRequired("Type", std::get<QualifiedCompound>(type));
}

template <>
struct llvm::yaml::MappingTraits<QualifiedType> {
  static void mapping(IO& io, QualifiedType& info) {
    map_qualified_type(io, info);
  }
};

template <>
struct llvm::yaml::MappingTraits<std::shared_ptr<Member>> {
  static void mapping(IO& io, std::shared_ptr<Member>& info) {
    io.mapRequired("Name", info->name);
    map_qualified_type(io, info->member);
  }
};

template <>
struct llvm::yaml::MappingTraits<location::SourceLocation> {
  static void mapping(IO& io, location::SourceLocation& info) {
    io.mapRequired("File", info.file);
    io.mapOptional("Function", info.function);
    io.mapRequired("Line", info.line);
  }
};

template <>
struct llvm::yaml::MappingTraits<LocatedType> {
  static void mapping(IO& io, LocatedType& info) {
    io.mapRequired("SourceLoc", info.location);
    map_qualified_type(io, info.type);
  }
};

template <>
struct llvm::yaml::SequenceTraits<QualifiedTypeList> {
  static size_t size(IO& io, QualifiedTypeList& seq) {
    return seq.size();
  }
  static QualifiedTypeList::value_type& element(IO& io, QualifiedTypeList& seq, size_t index) {
    if (index >= seq.size()) {
      seq.resize(index + 1);
    }
    return seq.at(index);
  }
};

template <>
struct llvm::yaml::MappingTraits<CompileUnitTypes> {
  static void mapping(IO& io, CompileUnitTypes& info) {
    io.mapRequired("CUName", info.name);
    io.mapOptional("Types", info.types);
  }
};

LLVM_YAML_IS_SEQUENCE_VECTOR(CompileUnitTypes)

namespace dimeta::io {
namespace detail {
template <typename T>
bool emit(llvm::raw_string_ostream& oss, const T& obj) {
  using namespace llvm;

  yaml::Output out(oss);
  out << const_cast<T&>(obj);

  return true;
}

template <typename T>
bool input(llvm::StringRef yaml, T& obj) {
  using namespace llvm;

  yaml::Input in(yaml);
  in >> obj;

  return true;
}
}  // namespace detail

bool emit(llvm::raw_string_ostream& oss, const QualifiedFundamental& compound) {
  return detail::emit(oss, compound);
}

bool input(llvm::StringRef yaml, QualifiedFundamental& compound) {
  return detail::input(yaml, compound);
}

bool emit(llvm::raw_string_ostream& oss, const QualifiedCompound& compound) {
  return detail::emit(oss, compound);
}

bool input(llvm::StringRef yaml, QualifiedCompound& compound) {
  return detail::input(yaml, compound);
}

bool emit(llvm::raw_string_ostream& oss, const LocatedType& type) {
  return detail::emit(oss, type);
}

bool input(llvm::StringRef yaml, LocatedType& type) {
  return detail::input(yaml, type);
}

bool emit(llvm::raw_string_ostream& oss, const CompileUnitTypes& cu_types) {
  return detail::emit(oss, cu_types);
}

bool input(llvm::StringRef yaml, CompileUnitTypes& cu_types) {
  return detail::input(yaml, cu_types);
}

bool emit(llvm::raw_string_ostream& oss, const CompileUnitTypeList& cu_types) {
  return detail::emit(oss, cu_types);
}

bool input(llvm::StringRef yaml, CompileUnitTypeList& cu_types) {
  return detail::input(yaml, cu_types);
}

}  // namespace dimeta::io