//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIMETA_H
#define DIMETA_DIMETA_H

#include <optional>
#include <variant>

namespace llvm {
class AllocaInst;
class CallBase;
class DILocalVariable;
class DIType;
class Value;
class GlobalVariable;
}  // namespace llvm

namespace dimeta {

using DimetaDIVar = std::variant<llvm::DILocalVariable*, llvm::GlobalVariable*>;

struct DimetaData {
  enum MemLoc { Stack = 0, Heap, Global };
  MemLoc location{Stack};
  std::optional<DimetaDIVar> di_variable{};   // if existing the named variable w.r.t. allocation
  std::optional<llvm::DIType*> entry_type{};  // determined to be the allocation including "pointer" DITypes
  std::optional<llvm::DIType*> base_type{};   // The base type (int, struct X...) of the allocated memory
  int pointer_level{0};                       // e.g., 1 -> int*, 2 -> int**, etc.
};

std::optional<DimetaData> type_for(const llvm::AllocaInst*);

std::optional<DimetaData> type_for(const llvm::CallBase*);

std::optional<DimetaData> type_for(const llvm::GlobalVariable*);

}  // namespace dimeta

#endif  // DIMETA_DIMETA_H
