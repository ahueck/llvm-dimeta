//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_GEP_H
#define DIMETA_GEP_H

#include <optional>

namespace llvm {
class DIType;
class GEPOperator;
class DIDerivedType;
}  // namespace llvm

namespace dimeta::gep {

struct GepIndexToType {
  std::optional<llvm::DIType*> type;
  std::optional<llvm::DIDerivedType*> member{};
  bool use_type{false};
};

GepIndexToType extract_gep_dereferenced_type(llvm::DIType* root, const llvm::GEPOperator& inst);

}  // namespace dimeta::gep

#endif  // DIMETA_GEP_H
