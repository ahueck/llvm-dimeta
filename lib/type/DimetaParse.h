//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIMETAPARSE_H
#define DIMETA_DIMETAPARSE_H

#include "DimetaData.h"

#include <optional>
#include <variant>

namespace llvm {
class DINode;
}

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

std::optional<DimetaParseResult> make_dimetadata(const llvm::DINode*);

}  // namespace dimeta::parser

#endif  // DIMETA_DIMETAPARSE_H
