//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIMETA_H
#define DIMETA_DIMETA_H

namespace llvm {
class AllocaInst;
class CallInst;
}  // namespace llvm

namespace dimeta {

void type_for(llvm::AllocaInst*);

void type_for(llvm::CallInst* call);

}  // namespace dimeta

#endif  // DIMETA_DIMETA_H
