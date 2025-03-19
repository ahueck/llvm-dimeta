//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIMETADATA_H
#define DIMETA_DIMETADATA_H

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace dimeta {

using Extent    = std::uint64_t;
using Offset    = std::uint64_t;
using ArraySize = std::uint64_t;

enum class Qualifier {
  kNone        = 0x0,
  kConst       = 0x1,
  kPtr         = 0x2,
  kRef         = 0x4,
  kPtrToMember = 0x8,
  kArray       = 0x10,
  kVector      = 0x20,
  kStatic      = 0x40
};

struct Member;
struct BaseClass;
using Members       = std::vector<std::shared_ptr<Member>>;
using Bases         = std::vector<std::shared_ptr<BaseClass>>;
using Offsets       = std::vector<Offset>;
using Sizes         = std::vector<Extent>;
using Qualifiers    = std::vector<Qualifier>;
using ArraySizeList = std::vector<ArraySize>;

struct CompoundType {
  // struct, union, class etc.
  enum Tag {
    kUnknown   = 0x0,
    kStruct    = 0x1,
    kClass     = 0x2,
    kUnion     = 0x4,
    kEnum      = 0x8,
    kEnumClass = 0x10,
  };

  std::string name;
  std::string identifier;
  Tag type;
  Extent extent;
  Offsets offsets;
  Sizes sizes;
  // Mapping: Base -> Compound
  Bases bases;
  // Mapping: Member -> name, QualType<[Compound, FundamentalType, (Padding)]>
  Members members;
  Members static_members;
};

struct FundamentalType {
  // int, double etc.
  enum Encoding {
    kUnknown      = 0,
    kFloat        = 1 << 0,
    kChar         = 1 << 1,
    kInt          = 1 << 2,
    kSigned       = 1 << 3,
    kUnsigned     = 1 << 4,
    kBool         = 1 << 5,
    kPadding      = 1 << 6,
    kVoid         = 1 << 7,
    kVtablePtr    = 1 << 8,
    kNullptr      = 1 << 9,
    kUTFChar      = 1 << 10,
    kComplex      = 1 << 11,
    kSignedChar   = kChar | kSigned,
    kUnsignedChar = kChar | kUnsigned,
    kSignedInt    = kInt | kSigned,
    kUnsignedInt  = kInt | kUnsigned,
  };

  std::string name;
  Extent extent;
  Encoding encoding{Encoding::kUnknown};
};

template <typename T>
struct QualType {
  T type{};
  ArraySizeList array_size;
  Qualifiers qual;
  std::string typedef_name;
  Extent vector_size{0};
  bool is_vector{false};
  bool is_forward_decl{false};
  bool is_recurring{false};
};

using QualifiedFundamental = QualType<FundamentalType>;
using QualifiedCompound    = QualType<CompoundType>;
using QualifiedType        = std::variant<QualifiedCompound, QualifiedFundamental>;
using QualifiedTypeList    = std::vector<QualifiedType>;
struct CompileUnitTypes {
  std::string name;
  QualifiedTypeList types;
};
using CompileUnitTypeList = std::vector<CompileUnitTypes>;

struct BaseClass {
  QualifiedCompound base{};
  // Byte offset from start of containing class:
  Offset offset{0};
  bool is_empty_base_class{false};
};

struct Member {
  std::string name;
  QualifiedType member;
};

namespace location {

struct SourceLocation {
  std::string file;
  std::string function;
  unsigned line{};
};

}  // namespace location

struct LocatedType {
  QualifiedType type;
  location::SourceLocation location;
};
}  // namespace dimeta

#endif  // DIMETA_DIMETADATA_H
