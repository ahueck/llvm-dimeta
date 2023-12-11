//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DimetaData.h"
#include "Util.h"

#include "llvm/ADT/StringRef.h"
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
    io.enumCase(value, "vtable_ptr", dimeta::FundamentalType::Encoding::kVtablePtr);
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
  }
};

LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(dimeta::Qualifier)

template <>
struct llvm::yaml::MappingTraits<QualifiedFundamental> {
  static void mapping(IO& io, QualifiedFundamental& info) {
    io.mapRequired("Fundamental", info.type);
    //    io.mapOptional("Array", info.array_size);
    map_optional_not_empty(io, "Array", info.array_size);
    map_optional_not_empty(io, "Qualifiers", info.qual);
    map_optional_not_empty(io, "Typedef", info.typedef_name);
  }
};

template <>
struct llvm::yaml::MappingTraits<QualifiedCompound> {
  static void mapping(IO& io, QualifiedCompound& info) {
    io.mapRequired("Compound", info.type);
    //    io.mapOptional("Array", info.array_size);
    map_optional_not_empty(io, "Array", info.array_size);
    //    io.mapOptional("Qualifiers", info.qual);
    map_optional_not_empty(io, "Qualifiers", info.qual);
    map_optional_not_empty(io, "Typedef", info.typedef_name);
  }
};

template <>
struct llvm::yaml::MappingTraits<std::shared_ptr<Member>> {
  static void mapping(IO& io, std::shared_ptr<Member>& info) {
    bool comp_t{false};

    io.mapRequired("Name", info->name);

    if (io.outputting()) {
      comp_t = bool(std::holds_alternative<QualifiedFundamental>(info->member));
    }
    io.mapRequired("Builtin", comp_t);
    if (!comp_t) {
      if (!io.outputting()) {
        info->member.emplace<QualifiedCompound>();
      }
      io.mapRequired("Type", std::get<QualifiedCompound>(info->member));
    } else {
      if (!io.outputting()) {
        info->member.emplace<QualifiedFundamental>();
      }
      io.mapRequired("Type", std::get<QualifiedFundamental>(info->member));
    }
  }
};

namespace dimeta::io {
bool emit(llvm::raw_string_ostream& oss, const QualifiedFundamental& compound) {
  using namespace llvm;

  yaml::Output out(oss);

  out << const_cast<QualifiedFundamental&>(compound);

  return true;
}

bool input(llvm::StringRef yaml, QualifiedFundamental& compound) {
  using namespace llvm;

  yaml::Input in(yaml);

  in >> compound;

  return true;
}

bool emit(llvm::raw_string_ostream& oss, const QualifiedCompound& compound) {
  using namespace llvm;

  yaml::Output out(oss);

  out << const_cast<QualifiedCompound&>(compound);

  return true;
}

bool input(llvm::StringRef yaml, QualifiedCompound& compound) {
  using namespace llvm;

  yaml::Input in(yaml);

  in >> compound;

  return true;
}
}  // namespace dimeta::io