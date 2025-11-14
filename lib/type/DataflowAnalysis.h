//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DATAFLOWANALYSIS_H
#define DIMETA_DATAFLOWANALYSIS_H

#include "llvm/ADT/SmallVector.h"

#include <optional>

namespace dimeta::dataflow {
struct ValuePath;
}  // namespace dimeta::dataflow

namespace llvm {
class CallBase;
class AllocaInst;
class Value;
}  // namespace llvm

namespace dimeta::dataflow {

llvm::SmallVector<dataflow::ValuePath, 4> type_for_heap_call(const llvm::CallBase* call);

llvm::SmallVector<dataflow::ValuePath, 4> path_from_alloca(const llvm::AllocaInst* alloca);

namespace fortran {
std::optional<llvm::Value*> shape_from_value(const llvm::Value* start);
bool passed_to_fortran_helper(const llvm::Value* start);
}  // namespace fortran

namespace experimental {
llvm::SmallVector<dataflow::ValuePath, 4> path_from_value(const llvm::Value*);
}  // namespace experimental

}  // namespace dimeta::dataflow

#endif  // DIMETA_DATAFLOWANALYSIS_H
