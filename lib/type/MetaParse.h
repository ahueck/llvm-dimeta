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

#include <memory>
#include <variant>

namespace dimeta::parser {

struct DimetaParseResult {
  QualifiedType type_;

  [[nodiscard]] bool hasCompound() const {
    return std::holds_alternative<QualifiedCompound>(type_);
  }

  [[nodiscard]] bool hasFundamental() const {
    return std::holds_alternative<QualifiedFundamental>(type_);
  }

  template <typename QualifiedType>
  std::optional<QualifiedType> getAs() const {
    if (std::holds_alternative<QualifiedType>(type_)) {
      return std::get<QualifiedType>(type_);
    }
    return {};
  }
};

class DITypeParser : public visitor::DINodeVisitor<DITypeParser> {
 private:
  struct MetaData;
  std::unique_ptr<MetaData> meta_tracker_;
  DimetaParseResult result_;

  template <typename QualType>
  void emplace_result(QualType type) {
    result_.type_.emplace<QualType>(type);
  }

 public:
  DITypeParser();
  ~DITypeParser();

  [[nodiscard]] const DimetaParseResult& getParsedType() const;

  bool visitBasicType(const llvm::DIBasicType* basic_type);

  bool visitDerivedType(const llvm::DIDerivedType* derived_type);

  bool visitCompositeType(const llvm::DICompositeType* composite_type);

  void leaveCompositeType(const llvm::DICompositeType* t);

  void leaveBasicType(const llvm::DIBasicType*);
};

}  // namespace dimeta::parser

#endif  // DIMETA_METAPARSE_H
