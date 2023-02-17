//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIMETA_H
#define DIMETA_DIMETA_H

#include "llvm/ADT/Optional.h"

namespace llvm {
class AllocaInst;
class CallInst;
class DILocalVariable;
}  // namespace llvm

namespace dimeta {

void type_for(llvm::AllocaInst*);

void type_for(llvm::CallInst* call);

llvm::Optional<llvm::DILocalVariable*> local_di_variable_for(llvm::AllocaInst*);

}  // namespace dimeta

#endif  // DIMETA_DIMETA_H
