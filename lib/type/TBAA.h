//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_TBAA_H
#define DIMETA_TBAA_H

// #include <llvm/IR/Instructions.h>
#include <optional>

namespace llvm {
class DIType;
class LoadInst;
}  // namespace llvm

namespace dimeta::dataflow {
class ValuePath;
}

namespace dimeta::tbaa {

std::optional<llvm::DIType*> resolve_tbaa(llvm::DIType* root, const dataflow::ValuePath& path);
std::optional<llvm::DIType*> resolve_tbaa(llvm::DIType* root, const llvm::LoadInst& load_inst);

}  // namespace dimeta::tbaa

#endif  // DIMETA_TBAA_H
