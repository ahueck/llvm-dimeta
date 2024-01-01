//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
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
  kNone  = 0x0,
  kConst = 0x1,
  kPtr   = 0x2,
  kRef   = 0x4,
};

struct Member;
struct BaseClass;
using Members     = std::vector<std::shared_ptr<Member>>;
using Bases       = std::vector<std::shared_ptr<BaseClass>>;
using Offsets     = std::vector<Offset>;
using MemberSizes = std::vector<Extent>;
using Qualifiers  = std::vector<Qualifier>;

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
  MemberSizes sizes;
  // Mapping: Base -> Compound
  Bases bases;
  // Mapping: Member -> name, QualType<[Compound, FundamentalType, (Padding)]>
  Members members;
};

struct FundamentalType {
  // int, double etc.
  enum Encoding {
    kUnknown      = 0x0,
    kFloat        = 0x1,
    kChar         = 0x2,
    kInt          = 0x4,
    kSigned       = 0x8,
    kUnsigned     = 0x10,
    kBool         = 0x12,
    kPadding      = 0x14,  // TODO: maybe make this explicit
    kVtablePtr    = 0x16,
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
  ArraySize array_size{0};  // TODO consider class (around QualType<T>) to model arrays
  Qualifiers qual{};
  std::string typedef_name{};
  bool recurrs{false};
};

using QualifiedFundamental = QualType<FundamentalType>;
using QualifiedCompound    = QualType<CompoundType>;
using QualifiedType        = std::variant<QualifiedCompound, QualifiedFundamental>;

struct BaseClass {
  QualifiedCompound base{};
};

struct Member {
  std::string name;
  QualifiedType member;
};

namespace location {

struct SourceLocation {
  std::string file{};
  std::string function{};
  unsigned line{};
};

struct LocatedType {
  QualifiedType type;
  SourceLocation location;
};

}  // namespace location

}  // namespace dimeta

#endif  // DIMETA_DIMETADATA_H
