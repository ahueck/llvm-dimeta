//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIFinder.h"

#include "Util.h"
#include "support/Logger.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/ilist_iterator.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <iterator>
#include <llvm/IR/DebugInfo.h>
#include <optional>

namespace llvm {
class DbgVariableIntrinsic;
}  // namespace llvm

namespace dimeta::difinder {

namespace compat {

namespace impl {
#if LLVM_VERSION_MAJOR > 16

template <typename DbgVar>
inline bool is_dbg_assign_impl(const DbgVar* dbg_var) {
#if LLVM_VERSION_MAJOR < 19
  return (dbg_var->getIntrinsicID() == llvm::Intrinsic::dbg_assign);
#else
  return (dbg_var->getType() == llvm::DbgVariableRecord::LocationType::Assign);
#endif
  return false;
}

template <typename DbgVar>
inline auto get_assigns_impl(const DbgVar* dbg_var) {
#if LLVM_VERSION_MAJOR < 19
  return llvm::at::getAssignmentInsts(llvm::dyn_cast<llvm::DbgAssignIntrinsic>(dbg_var));
#else
  return llvm::at::getAssignmentInsts(llvm::dyn_cast<llvm::DbgVariableRecord>(dbg_var));
#endif
}

#endif
}  // namespace impl

template <typename DbgVar>
inline bool is_dbg_assign(const DbgVar* dbg_var) {
#if LLVM_VERSION_MAJOR > 16
  return impl::is_dbg_assign_impl(dbg_var);
#else
  return false;
#endif
}

template <typename DbgVar>
std::optional<llvm::Value*> get_alloca_from_dbg_assign(const DbgVar* dbg_var) {
#if LLVM_VERSION_MAJOR > 16
  const auto assigns = impl::get_assigns_impl(dbg_var);
  for (llvm::Value* assign : assigns) {
    if (auto* store = llvm::dyn_cast<llvm::StoreInst>(assign)) {
      return store->getPointerOperand();
    }
  }
#endif
  return {};
}

template <typename DbgVar>
const llvm::Value* get_alloca_for(const DbgVar* dbg_var) {
#if LLVM_VERSION_MAJOR < 13
  return dbg_var->getVariableLocation();
#elif LLVM_VERSION_MAJOR <= 16
  return dbg_var->getVariableLocationOp(0);
#else
  if (is_dbg_assign(dbg_var)) {
    return get_alloca_from_dbg_assign(dbg_var).value_or(nullptr);
  }
  return dbg_var->getVariableLocationOp(0);
#endif
}

template <typename DbgVar>
llvm::DIExpression* get_expr_for(const DbgVar* dbg_var) {
  return dbg_var->getExpression();
}

}  // namespace compat

#if LLVM_VERSION_MAJOR < 19

std::optional<const llvm::DbgVariableIntrinsic*> find_intrinsic(const llvm::Instruction* ai) {
  auto& func = *ai->getFunction();
  for (auto& inst : llvm::instructions(func)) {
    if (auto* dbg = llvm::dyn_cast<llvm::DbgVariableIntrinsic>(&inst)) {
      if (compat::get_alloca_for(dbg) == ai) {
        return dbg;
      }
    }
  }
  return {};
}

#else

std::optional<const llvm::DbgVariableRecord*> find_intrinsic(const llvm::Instruction* root) {
  auto& func = *root->getFunction();
  for (auto const& inst : llvm::instructions(func)) {
    for (llvm::DbgVariableRecord& var : filterDbgVars(inst.getDbgRecordRange())) {
      // LOG_DEBUG(var)
      if (compat::get_alloca_for(&var) == root) {
        // LOG_DEBUG(var)
        return &var;
      }
    }
  }
  return {};
}

#endif

std::optional<llvm::DILocalVariable*> find_local_variable(const llvm::Instruction* ai, bool) {
  using namespace llvm;
  //  DebugInfoFinder di_finder;
  const auto find_di_var = [&](auto* ai) -> std::optional<DILocalVariable*> {
    auto intrinsic = find_intrinsic(ai);
    if (intrinsic) {
      return intrinsic.value()->getVariable();
    }
    return {};
  };
  auto result = find_di_var(ai);
  if (!result) {
    for (auto* user : ai->users()) {
      if (auto bcast = llvm::dyn_cast<llvm::BitCastInst>(user)) {
        result = find_di_var(bcast);
        if (result) {
          break;
        }
      }
      if (auto load = llvm::dyn_cast<llvm::LoadInst>(user)) {
        result = find_di_var(load);
        if (result) {
          break;
        }
      }
    }
  }
  return result;
}

std::optional<llvm::DILocation*> find_location(const llvm::Instruction* inst) {
  if (const auto& location = inst->getDebugLoc()) {
    return location.get();
  }

  auto dbg_intrinsic = find_intrinsic(inst);
  if (dbg_intrinsic) {
    return dbg_intrinsic.value()->getDebugLoc().get();
  }

  return {};
}

std::optional<LocalAccessData> get_array_access_assignment(const llvm::CallBase* call) {
  auto intrinsic = find_intrinsic(call);
  if (intrinsic) {
    auto* expr = compat::get_expr_for(intrinsic.value());
    if (auto* array = llvm::dyn_cast<llvm::DIExpression>(expr)) {
      if (array->isFragment()) {
        return {LocalAccessData{intrinsic.value()->getVariable(), array}};
      }
    }
    return {LocalAccessData{intrinsic.value()->getVariable(), {}}};
  }
  return {};
}

}  // namespace dimeta::difinder
