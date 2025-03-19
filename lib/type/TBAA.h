//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_TBAA_H
#define DIMETA_TBAA_H

#include <optional>

namespace llvm {
class DIType;
class Instruction;
}  // namespace llvm

namespace dimeta::tbaa {

std::optional<llvm::DIType*> resolve_tbaa(llvm::DIType* root, const llvm::Instruction& inst);

}  // namespace dimeta::tbaa

#endif  // DIMETA_TBAA_H
