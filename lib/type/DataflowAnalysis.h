//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef LIB_TYPE_DATAFLOWANALYSIS
#define LIB_TYPE_DATAFLOWANALYSIS

#include "llvm/ADT/SmallVector.h"

#include <cstdint>
#include <llvm/IR/Instruction.h>
#include <optional>
#include <vector>

namespace dimeta {
struct ShapeData;
}

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
llvm::SmallVector<dataflow::ValuePath, 4> path_from_instruction(const llvm::Instruction* inst);

namespace fortran {
std::optional<ShapeData> shape_from_value(const llvm::Value* start);
bool passed_to_fortran_helper(const llvm::Value* start);
}  // namespace fortran

namespace experimental {
llvm::SmallVector<dataflow::ValuePath, 4> path_from_value(const llvm::Value*);
}  // namespace experimental

}  // namespace dimeta::dataflow

#endif /* LIB_TYPE_DATAFLOWANALYSIS */
