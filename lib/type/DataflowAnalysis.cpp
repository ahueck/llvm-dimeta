//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DataflowAnalysis.h"

#include "DefUseAnalysis.h"

#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <optional>

namespace llvm {
class Argument;
class GlobalVariable;
class Module;
}  // namespace llvm

namespace dimeta::dataflow {

struct MallocAnchorMatcher {
  const llvm::CallBase* malloc_like_call{nullptr};
  llvm::SmallVector<ValuePath, 4> anchors;

  explicit MallocAnchorMatcher(const llvm::CallBase* call) : malloc_like_call{call} {
  }

  [[nodiscard]] const llvm::Module* getModule() const;

  [[nodiscard]] const llvm::Function* getFunction() const;

  auto operator()(const ValuePath& path) -> decltype(DefUseChain::kContinue);
};

struct MallocTargetMatcher {
  llvm::SmallVector<dataflow::ValuePath, 4> types_path;

  auto operator()(const dataflow::ValuePath& path) -> decltype(dataflow::DefUseChain::kContinue);
};

struct MallocBacktrackSearch {
  using ValueRange = llvm::TinyPtrVector<const llvm::Value*>;

  auto operator()(const dataflow::ValuePath& path) -> std::optional<ValueRange>;
};

llvm::SmallVector<dataflow::ValuePath, 4> type_for_heap_call(const llvm::CallBase* call) {
  using namespace llvm;
  using dataflow::DefUseChain;
  using dataflow::ValuePath;
  using namespace dataflow;

  auto should_search = [&](const ValuePath&) -> bool { return true; };
  DefUseChain value_traversal;

  // forward: find pathes to anchor (store, ret, func call etc.) from malloc-like
  MallocAnchorMatcher malloc_anchor_finder{call};
  value_traversal.traverse(call, malloc_anchor_finder, should_search);

  // backward: find pathes from anchor (store) to alloca/argument/global etc.
  MallocTargetMatcher malloc_anchor_backtrack;
  MallocBacktrackSearch backtracker_search;

  assert(!malloc_anchor_finder.anchors.empty() && "Anchor should not be empty");
  llvm::SmallVector<dataflow::ValuePath, 4> ditype_paths;

  for (const auto& anchor_path : malloc_anchor_finder.anchors) {
    if (isa<StoreInst>(anchor_path.value())) {
      //      dbgs() << "Traverse " << anchor_path.value() << "\n";
      value_traversal.traverse_custom(anchor_path.value(), malloc_anchor_backtrack, should_search, backtracker_search);
      for (const auto& backtrack_path : malloc_anchor_backtrack.types_path) {
        ditype_paths.emplace_back(backtrack_path);
      }
      continue;
    }
    ditype_paths.emplace_back(anchor_path);
  }

  llvm::dbgs() << "Paths to types: \n";
  for (const auto& path : ditype_paths) {
    llvm::dbgs() << "  T: " << path << "\n";
  }

  return ditype_paths;
}

auto MallocBacktrackSearch::operator()(const dataflow::ValuePath& path) -> std::optional<ValueRange> {
  using namespace llvm;
  using dataflow::DefUseChain;
  using dataflow::ValuePath;

  ValueRange result;

  if (auto const_expr = llvm::dyn_cast<llvm::ConstantExpr>(path.value())) {
    if (const_expr->getOpcode() == Instruction::GetElementPtr) {
      if (auto op = llvm::dyn_cast<llvm::GEPOperator>(const_expr)) {
        //          dbgs() << "Gep Constant casted. ";
        //          dbgs() << "gep of " << *op->getPointerOperand() << "\n";
        result.push_back(op->getPointerOperand());
      }
      return result;
    }
  }

  const auto* inst = dyn_cast<Instruction>(path.value());
  if (inst == nullptr) {
    return {};
  }

  dbgs() << "Backtracking " << *inst << "\n";

  switch (inst->getOpcode()) {
    case Instruction::Store: {
      result.push_back(llvm::dyn_cast<StoreInst>(inst)->getPointerOperand());
      return result;
    }
    case Instruction::Load: {
      result.push_back(llvm::dyn_cast<LoadInst>(inst)->getPointerOperand());
      return result;
    }
    case Instruction::GetElementPtr: {
      result.push_back(llvm::dyn_cast<GetElementPtrInst>(inst)->getPointerOperand());
      return result;
    }
    case Instruction::BitCast: {
      result.push_back(llvm::dyn_cast<llvm::BitCastInst>(inst)->getOperand(0));
      return result;
    }
    case Instruction::PHI: {
      auto* phi = llvm::dyn_cast<PHINode>(inst);
      for (auto& incoming : phi->incoming_values()) {
        dbgs() << "  > Backtracking phi incoming " << *incoming.get() << "\n";
        result.push_back(incoming.get());
      }
      return result;
    }
  }
  return {};
}

auto MallocTargetMatcher::operator()(const dataflow::ValuePath& path) -> decltype(dataflow::DefUseChain::kContinue) {
  using namespace llvm;
  using dataflow::DefUseChain;
  using dataflow::ValuePath;

  const auto* const value = path.value();
  // Handle path to alloca -> can extract type
  if (const auto* inst = dyn_cast<Instruction>(value)) {
    if (isa<IntrinsicInst>(inst)) {
      return DefUseChain::kSkip;
    }
    if (isa<AllocaInst>(inst)) {
      types_path.emplace_back(path);
      return DefUseChain::kSkip;  // maybe kCancel is better?
    }
  }

  if (isa<llvm::Argument>(value)) {
    types_path.emplace_back(path);
    return DefUseChain::kSkip;
  }

  if (llvm::isa<llvm::GlobalVariable>(value)) {
    types_path.emplace_back(path);
    return DefUseChain::kSkip;
  }

  if (const auto* const_expr = llvm::dyn_cast<llvm::ConstantExpr>(value)) {
    llvm::dbgs() << "Unsupported: Found constant expr: " << *const_expr << "\n";
  }

  return DefUseChain::kContinue;
}

const llvm::Module* MallocAnchorMatcher::getModule() const {
  return getFunction()->getParent();
}

const llvm::Function* MallocAnchorMatcher::getFunction() const {
  return malloc_like_call->getParent()->getParent();
}

auto MallocAnchorMatcher::operator()(const ValuePath& path) -> decltype(DefUseChain::kContinue) {
  using namespace llvm;
  using dataflow::DefUseChain;
  using dataflow::ValuePath;
  const auto* inst = dyn_cast<Instruction>(path.value());
  if (!inst) {
    return DefUseChain::kSkip;
  }

  if (isa<IntrinsicInst>(inst)) {
    return DefUseChain::kSkip;
  }

  switch (inst->getOpcode()) {
    case Instruction::Ret: {
      outs() << "Matched Ret! " << *inst << "\n";
      outs() << "  Path: " << path << "\n";
      anchors.push_back(path);
      return DefUseChain::kCancel;
    }
    case Instruction::Store: {
      outs() << "Matched store! " << *inst << "\n";
      outs() << "  Path: " << path << "\n";
      anchors.push_back(path);
      return DefUseChain::kCancel;
    }
    case Instruction::Call:  // NOLINT
      [[fallthrough]];
    case Instruction::CallBr:
      [[fallthrough]];
    case Instruction::Invoke: {
      if (path.only_start()) {
        return DefUseChain::kContinue;
      }
      outs() << "Matched call! " << *inst << "\n";
      outs() << "  Path: " << path << "\n";

      anchors.push_back(path);

      //        const auto* call_inst = llvm::cast<CallBase>(inst);
      //        const auto* called_f  = call_inst->getCalledFunction();
      //        if (called_f != nullptr) {
      //          outs() << "Matched callbase! " << *called_f << "\n";
      //          // Argument passed to current call:
      //          const auto* arg_val = path.previous_value();
      //          assert(arg_val != nullptr && "Previous value should be argument to some function!");
      //
      //          // Argument number:
      //          const auto* const arg_pos = llvm::find_if(
      //              call_inst->args(), [&arg_val](const auto& arg_use) -> bool { return arg_use.get() == arg_val;
      //              });
      //          if (arg_pos == call_inst->arg_end()) {
      //            return DefUseChain::kContinue;
      //          }
      //          auto arg_num = std::distance(call_inst->arg_begin(), arg_pos);
      //          // Extract debug info from function at arg_num:
      //          if (auto* sub_program = called_f->getSubprogram(); sub_program != nullptr) {
      //            const auto sub_prog_arg_pos = arg_num + 1;
      //            auto types_of_subprog       = sub_program->getType()->getTypeArray();
      //            assert((types_of_subprog.size() > sub_prog_arg_pos) && "Type array smaller than arg num!");
      //            auto* type = types_of_subprog[sub_prog_arg_pos];
      //            util::DIPrinter printer(llvm::outs(), getModule());
      //            printer.traverseType(type);
      //            return DefUseChain::kCancel;
      //          }
      //        }
      return DefUseChain::kContinue;
    }
  }
  return DefUseChain::kContinue;
}

}  // namespace dimeta::dataflow