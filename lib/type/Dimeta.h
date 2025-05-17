//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIMETA_H
#define DIMETA_DIMETA_H

#include "DimetaData.h"

#include <optional>
#include <variant>

namespace llvm {
class AllocaInst;
class CallBase;
class DILocalVariable;
class DIGlobalVariable;
class DIType;
class DILocation;
class Value;
class GlobalVariable;
class Module;
}  // namespace llvm

namespace dimeta {

using DimetaDIVariable = std::variant<llvm::DILocalVariable*, llvm::DIGlobalVariable*>;

struct DimetaData {
  enum MemLoc { kStack = 0, kHeap, kGlobal };
  MemLoc memory_location{kStack};
  std::optional<DimetaDIVariable> di_variable{};   // if existing the named variable w.r.t. allocation
  std::optional<llvm::DIType*> entry_type{};       // determined to be the allocation including "pointer" DITypes
  std::optional<llvm::DIType*> base_type{};        // The base type (int, struct X...) of the allocated memory
  std::optional<llvm::DILocation*> di_location{};  // Loc of call (malloc etc.)/alloca. Not set for global
  int pointer_level{0};                            // e.g., 1 -> int*, 2 -> int**, etc.
};

std::optional<DimetaData> type_for(const llvm::AllocaInst*);

std::optional<DimetaData> type_for(const llvm::CallBase*);

std::optional<DimetaData> type_for(const llvm::GlobalVariable*);

std::optional<location::SourceLocation> location_for(const DimetaData&);

std::optional<LocatedType> located_type_for(const DimetaData&);

std::optional<LocatedType> located_type_for(const llvm::AllocaInst*);

std::optional<LocatedType> located_type_for(const llvm::CallBase*);

std::optional<LocatedType> located_type_for(const llvm::GlobalVariable*);

std::optional<CompileUnitTypeList> compile_unit_types(const llvm::Module*);

namespace experimental {
std::optional<QualifiedType> type_for(const llvm::Value*);

}  // namespace experimental

}  // namespace dimeta

#endif  // DIMETA_DIMETA_H
