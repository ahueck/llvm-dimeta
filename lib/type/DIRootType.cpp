//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIRootType.h"

#include "DIFinder.h"
#include "DataflowAnalysis.h"
#include "DefUseAnalysis.h"
#include "Dimeta.h"
#include "MemoryOps.h"
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

namespace dimeta::root {

namespace helper {

std::optional<llvm::DIType*> type_of_store_to_call(const dataflow::ValuePath& path, const llvm::Function* called_f,
                                                   const llvm::CallBase* call_inst) {
  if (auto* leaf_value = path.start_value().value_or(nullptr); leaf_value != nullptr) {
    LOG_DEBUG("Looking at start value of path: " << *leaf_value)
    // Here we look at if we store to a function that returns pointer/ref,
    // indicating return of call is the type we need:
    if (auto* store = llvm::dyn_cast<llvm::StoreInst>(leaf_value)) {
      // We have a final store and root w.r.t. call, hence, assume return type is the relevant root DIType:
      if (auto* sub_program = called_f->getSubprogram(); sub_program != nullptr) {
        auto types_of_subprog = sub_program->getType()->getTypeArray();
        assert(types_of_subprog.size() > 0 && "Need the return type of the function");
        auto* return_type = types_of_subprog[0];
        LOG_DEBUG("Found return type " << log::ditype_str(return_type))
        return return_type;
      }

      LOG_DEBUG("Function has no subProgram to query, trying di_local finder")
      auto di_local = difinder::find_local_variable(call_inst);
      if (di_local) {
        // TODO: for now we ignore local vars with name "this", as these are auto generated
        auto is_this_var = di_local.value()->getName() == "this";
        if (is_this_var) {
          LOG_DEBUG("'this' local variable as store target unsupported.")
          return {};
        }
        LOG_DEBUG("Found local variable " << log::ditype_str(di_local.value()))
        return di_local.value()->getType();
      }
    }
  }
  return {};
}

std::optional<llvm::DIType*> type_of_call_argument(const dataflow::ValuePath& path, const llvm::Function* called_f,
                                                   const llvm::CallBase* call_inst) {
  // Argument passed to current call:
  const auto* arg_val = path.previous_value().value_or(nullptr);
  assert(arg_val != nullptr && "Previous value should be argument to some function!");
  // Argument number:
  const auto* const arg_pos =
      llvm::find_if(call_inst->args(), [&arg_val](const auto& arg_use) -> bool { return arg_use.get() == arg_val; });

  if (arg_pos == std::end(call_inst->args())) {
    LOG_DEBUG("Could not find arg position for " << *arg_val)
    return {};
  }
  auto arg_num = std::distance(call_inst->arg_begin(), arg_pos);
  LOG_DEBUG("Looking at arg pos " << arg_num)
  // Extract debug info from function at arg_num:
  if (auto* sub_program = called_f->getSubprogram(); sub_program != nullptr) {
    // DI-types of a subprog. include return type at pos 0, hence + 1:
    const auto sub_prog_arg_pos = arg_num + 1;
    auto types_of_subprog       = sub_program->getType()->getTypeArray();
    assert((types_of_subprog.size() > sub_prog_arg_pos) && "Type array smaller than arg num!");
    auto* type = types_of_subprog[sub_prog_arg_pos];
    LOG_DEBUG("Found DIType at arg pos " << log::ditype_str(type))
    return type;
  }
  LOG_DEBUG("Did not find arg pos")
  return {};
}

}  // namespace helper

std::optional<llvm::DIType*> find_type_root(const dataflow::ValuePath& path) {
  using namespace llvm;
  const auto* root_value = path.value().value_or(nullptr);
  if (!root_value) {
    return {};
  }
  LOG_DEBUG("Root value is " << *root_value)

  if (const auto* ret = dyn_cast<ReturnInst>(root_value)) {
    auto* sub_prog  = ret->getFunction()->getSubprogram();
    auto type_array = sub_prog->getType()->getTypeArray();
    if (type_array.size() > 0) {
      return {*type_array.begin()};
    }
    return {};
  }

  if (const auto* alloca = dyn_cast<AllocaInst>(root_value)) {
    auto local_di_var = difinder::find_local_variable(alloca);
    if (local_di_var) {
      return local_di_var.value()->getType();
    }

    // see test heap_case_inheritance.cpp (e.g., returns several objects as base class pointer):
    // TODO: check if that ever applies to C, should probably only execute for C++ codes.
    LOG_DEBUG("Dataflow analysis of alloca")
    auto paths_from_alloca = dataflow::path_from_alloca(alloca);
    for (auto& path : paths_from_alloca) {
      LOG_DEBUG("Path from alloca " << path)
      auto type_of_alloca = find_type_root(path);
      if (type_of_alloca) {
        return type_of_alloca;
      }
    }

    return {};
  }

  if (const auto* call_inst = llvm::dyn_cast<CallBase>(root_value)) {
    LOG_DEBUG("Root is a call")
    const auto* called_f = call_inst->getCalledFunction();
    if (called_f == nullptr) {
      LOG_DEBUG("Called function not found for call base " << *call_inst)
      return {};
    }

    dimeta::memory::MemOps ops;
    if (ops.allocKind(called_f->getName())) {
      // see test c/heap_tachyon_mock_images.c
      LOG_DEBUG("Root is malloc-like call")
      // TODO ask for type of newlike call here!
      auto extracted_type = type_for(call_inst);
      if (!extracted_type) {
        LOG_DEBUG("Failed to collect DI data for " << called_f->getName())
        return {};
      }

      return extracted_type->entry_type;
    }

    auto store_function_type = helper::type_of_store_to_call(path, called_f, call_inst);
    if (store_function_type) {
      return store_function_type;
    }

    auto type_of_call_arg = helper::type_of_call_argument(path, called_f, call_inst);
    if (!type_of_call_arg) {
      LOG_DEBUG("Did not find arg pos")
      return {};
    }
    return type_of_call_arg;
  }

  if (const auto* global_variable = llvm::dyn_cast<llvm::GlobalVariable>(root_value)) {
    auto dbg_md = global_variable->getMetadata("dbg");
    if (!dbg_md) {
      return {};
    }
    if (auto* global_expression = llvm::dyn_cast<llvm::DIGlobalVariableExpression>(dbg_md)) {
      return global_expression->getVariable()->getType();
    }
    return {};
  }

  if (const auto* argument = llvm::dyn_cast<llvm::Argument>(root_value)) {
    if (auto* subprogram = argument->getParent()->getSubprogram(); subprogram != nullptr) {
      const auto type_array = subprogram->getType()->getTypeArray();
      const auto arg_pos    = [&](const auto arg_num) {
        if (argument->hasStructRetAttr()) {
          return arg_num; // see test cpp/heap_lhs_function_opt_nofwd.cpp
        }
        return arg_num + 1;
      }(argument->getArgNo());

      LOG_FATAL(*subprogram << " " << *argument)
      LOG_FATAL("Arg data: " << argument->getArgNo() << " Type num operands: " << type_array->getNumOperands())
      assert(arg_pos < type_array.size() && "Arg position greater than DI type array of subprogram!");
      return type_array[arg_pos];
    }

    return {};
  }

  if (const auto* const_expr = llvm::dyn_cast<llvm::ConstantExpr>(root_value)) {
    LOG_DEBUG("ConstantExpr unsupported");
  }

  LOG_DEBUG("No matching value found for " << *root_value);
  return {};
}

}  // namespace dimeta::root