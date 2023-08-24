//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DATAFLOWANALYSIS_H
#define DIMETA_DATAFLOWANALYSIS_H

#include "DefUseAnalysis.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"

namespace dimeta {
namespace dataflow {
struct ValuePath;
}  // namespace dataflow
}  // namespace dimeta
namespace llvm {
class CallBase;
}  // namespace llvm

namespace dimeta::dataflow {

llvm::SmallVector<dataflow::ValuePath, 4> type_for_heap_call(const llvm::CallBase* call);

}  // namespace dimeta::dataflow

#endif  // DIMETA_DATAFLOWANALYSIS_H
