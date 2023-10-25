//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_GEP_H
#define DIMETA_GEP_H

// #include "llvm/IR/Operator.h"

#include <optional>

namespace llvm {
class DIType;
class GEPOperator;
}  // namespace llvm

namespace dimeta::gep {

std::optional<llvm::DIType*> extract_gep_deref_type(llvm::DIType* root, const llvm::GEPOperator& inst);

}  // namespace dimeta::gep

#endif  // DIMETA_GEP_H
