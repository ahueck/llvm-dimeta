//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIFINDER_H
#define DIMETA_DIFINDER_H

#include <llvm/IR/DebugInfoMetadata.h>
#include <optional>

namespace llvm {
class DIType;
class DbgVariableIntrinsic;
class DILocalVariable;
class DILocation;
class CallBase;
class Instruction;
class DIExpression;
}  // namespace llvm

namespace dimeta::difinder {

struct LocalAccessData {
  llvm::DILocalVariable* var;
  std::optional<llvm::DIExpression*> array_access;
};

std::optional<llvm::DILocalVariable*> find_local_variable(const llvm::Instruction* ai, bool bitcast_search = false);

std::optional<llvm::DILocation*> find_location(const llvm::Instruction* inst);

std::optional<LocalAccessData> get_array_access_assignment(const llvm::CallBase* call);

}  // namespace dimeta::difinder

#endif
