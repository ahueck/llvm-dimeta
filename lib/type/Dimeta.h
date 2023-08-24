//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIMETA_H
#define DIMETA_DIMETA_H

#include <optional>

namespace llvm {
class AllocaInst;
class CallBase;
class DILocalVariable;
class DIType;
class Value;
}  // namespace llvm

namespace dimeta {

std::optional<llvm::DILocalVariable*> type_for(const llvm::AllocaInst*);

std::optional<llvm::DIType*> type_for(const llvm::CallBase*);

}  // namespace dimeta

#endif  // DIMETA_DIMETA_H
