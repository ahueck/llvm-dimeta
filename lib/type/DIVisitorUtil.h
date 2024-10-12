//  llvm-dimeta library
//  Copyright (c) 2022-2024 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIVISITORUTILS_H
#define DIMETA_DIVISITORUTILS_H

namespace llvm {
class DINode;
class raw_ostream;
class Module;
}  // namespace llvm

namespace dimeta::visitor::util {

void print_dinode(llvm::DINode*, llvm::raw_ostream&, llvm::Module* m = nullptr);

// void print_dinode_semantic(llvm::DINode*, llvm::raw_ostream&);

}  // namespace dimeta::visitor::util

#endif  // DIMETA_DIVISITORUTILS_H
