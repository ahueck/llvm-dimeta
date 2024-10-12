//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DataflowAnalysis.h"

#include "DefUseAnalysis.h"
#include "support/Logger.h"

#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Module.h"
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
  llvm::SmallVector<ValuePath, 4> anchors;

  auto operator()(const ValuePath& path) -> decltype(DefUseChain::kContinue);
};

struct MallocTargetMatcher {
  llvm::SmallVector<ValuePath, 4> types_path;

  auto operator()(const ValuePath& path) -> decltype(DefUseChain::kContinue);
};

struct MallocBacktrackSearch {
  using ValueRange = llvm::TinyPtrVector<const llvm::Value*>;

  auto operator()(const ValuePath& path) -> std::optional<ValueRange>;
};

llvm::SmallVector<ValuePath, 4> type_for_heap_call(const llvm::CallBase* call) {
  using namespace llvm;

  auto should_search = [&](const ValuePath&) -> bool { return true; };
  DefUseChain value_traversal;

  // forward: find paths to anchor (store, ret, func call etc.) from malloc-like
  LOG_DEBUG("Find heap-call to anchor w.r.t. " << *call)
  // Anchor can be anything like function call "malloc -> .. -> foo(malloc)" or "malloc -> .. -> store" etc.
  MallocAnchorMatcher malloc_forward_anchor_finder;
  value_traversal.traverse(call, malloc_forward_anchor_finder, should_search);

  // backward: find paths from anchor (store) to alloca/argument/global etc.
  MallocTargetMatcher malloc_anchor_backtrack;
  MallocBacktrackSearch backtrack_search_dir_fn;

  assert(!malloc_forward_anchor_finder.anchors.empty() && "Anchor should not be empty");
  llvm::SmallVector<ValuePath, 4> ditype_paths;

  for (const auto& anchor_path : malloc_forward_anchor_finder.anchors) {
    LOG_DEBUG("Current anchor path " << anchor_path)
    if (auto* store_inst = dyn_cast_or_null<StoreInst>(anchor_path.value().value_or(nullptr))) {
      if (store_inst->getPointerOperand() == call) {
        // see test heap_lulesh_domain_mock.cpp with opt -O3
        continue;
      }
      LOG_DEBUG("Backtracking from anchor " << *anchor_path.value())
      //      dbgs() << "Traverse " << anchor_path.value() << "\n";
      value_traversal.traverse_custom(anchor_path.value().value(), malloc_anchor_backtrack, should_search,
                                      backtrack_search_dir_fn);
      for (const auto& backtrack_path : malloc_anchor_backtrack.types_path) {
        LOG_DEBUG("Found backtrack path " << backtrack_path)
        ditype_paths.emplace_back(backtrack_path);
      }
      continue;
    }
    ditype_paths.emplace_back(anchor_path);
  }

  LOG_DEBUG("\n")
  LOG_DEBUG("Final paths to types:");
  for (const auto& path : ditype_paths) {
    LOG_DEBUG("  T: " << path);
  }

  return ditype_paths;
}

llvm::SmallVector<ValuePath, 4> path_from_alloca(const llvm::AllocaInst* alloca) {
  using namespace llvm;

  auto should_search = [&](const ValuePath&) -> bool { return true; };
  DefUseChain value_traversal;

  MallocAnchorMatcher malloc_forward_anchor_finder;
  value_traversal.traverse(alloca, malloc_forward_anchor_finder, should_search);
  return malloc_forward_anchor_finder.anchors;
}

auto MallocBacktrackSearch::operator()(const ValuePath& path) -> std::optional<ValueRange> {
  // Backtracks form malloc target (a store) to, e.g., argument/global/etc.
  using namespace llvm;

  ValueRange result;

  if (auto const_expr = llvm::dyn_cast_or_null<llvm::ConstantExpr>(path.value().value_or(nullptr))) {
    if (const_expr->getOpcode() == Instruction::GetElementPtr) {
      if (auto op = llvm::dyn_cast<llvm::GEPOperator>(const_expr)) {
        result.push_back(op->getPointerOperand());
      }
      return result;
    }
    if (const_expr->getOpcode() == Instruction::BitCast) {
      // #if LLVM_VERSION_MAJOR < 15
      LOG_DEBUG("Found constant bitcast from value" << *const_expr->getOperand(0))
      result.push_back(const_expr->getOperand(0));
      // #endif
      return result;
    }
    LOG_ERROR("Unsupported ConstantExpr for path generation " << *const_expr)
  }

  const auto* inst = dyn_cast_or_null<Instruction>(path.value().value_or(nullptr));
  if (inst == nullptr) {
    return {};
  }

  LOG_DEBUG("Backtracking from " << *inst);

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
        LOG_DEBUG("  > Backtracking phi incoming " << *incoming.get());
        result.push_back(incoming.get());
      }
      return result;
    }
      //    case Instruction::Call: {
      //      LOG_DEBUG("Handle call inst")
      //      result.push_back(inst);
      //      return result;
      //    }
  }
  return {};
}

auto MallocTargetMatcher::operator()(const ValuePath& path) -> decltype(DefUseChain::kContinue) {
  // This builds the path (backtrack) from store to LHS target (argument, alloca etc.)
  using namespace llvm;

  const auto current_value = path.value();

  if(!current_value) {
    LOG_DEBUG("Current value is null, skipping")
    return DefUseChain::kSkip;
  }

  const auto* value = current_value.value();

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

  if (llvm::isa<llvm::Argument>(value)) {
    types_path.emplace_back(path);
    return DefUseChain::kSkip;
  }

  if (llvm::isa<llvm::GlobalVariable>(value)) {
    types_path.emplace_back(path);
    return DefUseChain::kSkip;
  }

  if (llvm::isa<llvm::CallBase>(value)) {
    types_path.emplace_back(path);
    return DefUseChain::kSkip;
  }

  //  if (const auto* const_expr = llvm::dyn_cast<llvm::ConstantExpr>(value)) {
  //    if (const_expr->isCast()) {
  //      LOG_DEBUG("Matcher adds constantexpr path " << path)
  //      types_path.emplace_back(path);
  //    } else {
  //      LOG_DEBUG("Unsupported: Found constant expr: " << *const_expr);
  //    }
  //  }

  return DefUseChain::kContinue;
}

// const llvm::Module* MallocAnchorMatcher::getModule() const {
//   return getFunction()->getParent();
// }
//
// const llvm::Function* MallocAnchorMatcher::getFunction() const {
//   return base_inst->getParent()->getParent();
// }

auto MallocAnchorMatcher::operator()(const ValuePath& path) -> decltype(DefUseChain::kContinue) {
  using namespace llvm;

  const auto current_value = path.value();
  if (!current_value) {
    return DefUseChain::kSkip;
  }

  const auto* inst = dyn_cast<Instruction>(current_value.value());
  if (!inst) {
    return DefUseChain::kSkip;
  }

  if (llvm::isa<IntrinsicInst>(inst)) {
    return DefUseChain::kSkip;
  }

  switch (inst->getOpcode()) {
    case Instruction::Ret: {
      anchors.push_back(path);
      return DefUseChain::kCancel;
    }
    case Instruction::Store: {
      const auto* store = cast<StoreInst>(inst);
      if (store->getPointerOperand() != path.start_value()) {
        anchors.push_back(path);
      } else {
        LOG_DEBUG("Store to allocated object, skipping. " << *store)
      }

      // kSkip instead of kCancel, as multiple stores can be present for a "malloc-like" call, see test
      // "heap_lulesh_domain_mock.cpp" at higher optim:
      return DefUseChain::kSkip;
    }
    case Instruction::GetElementPtr: {
      // Currently, we do not care about "new -> gep -> ...",
      // since this may be C++ init of the allocated object
      return DefUseChain::kSkip;
    }
    case Instruction::Call:  // NOLINT
      [[fallthrough]];
    case Instruction::CallBr:
      [[fallthrough]];
    case Instruction::Invoke: {
      if (path.only_start()) {
        return DefUseChain::kContinue;
      }

      // TODO Maybe check if we have a constructor call here, and not take it if it is?
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