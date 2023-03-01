//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "Dimeta.h"

#include "DIVisitor.h"
#include "MetaIO.h"
#include "MetaParse.h"

#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Support/raw_ostream.h"

#if LLVM_VERSION_MAJOR == 10
// For FindDbgAddrUses:
#include "llvm/Transforms/Utils/Local.h"
#endif

namespace dimeta {

namespace compat {
template <typename DbgVar>
llvm::Value* get_alloca_for(const DbgVar* dbg_var) {
#if LLVM_VERSION_MAJOR < 13
  return dbg_var->getVariableLocation();
#else
  return dbg_var->getVariableLocationOp(0);
#endif
}
}  // namespace compat

void type_for_inst(llvm::Instruction* inst) {
  using namespace llvm;

  DebugInfoFinder di_finder;

  util::DISemPrinter printer(llvm::errs());
  util::DIPrinter printer2(llvm::errs());

  const auto p = [](StringRef name, auto node) {
    llvm::errs() << name << ": ";
    if (node == nullptr) {
      llvm::errs() << "<null>";
    } else {
      node->dump();
    }
  };

  const auto find_di_var = [&](auto* ai) -> llvm::Optional<DILocalVariable*> {
    auto& func = *ai->getFunction();
    for (auto& inst : llvm::instructions(func)) {
      if (auto* dbg = dyn_cast<DbgValueInst>(&inst)) {
        if (compat::get_alloca_for(dbg) == ai) {
          di_finder.processInstruction(*ai->getModule(), inst);
          return dbg->getVariable();
        }
      }

      if (DbgVariableIntrinsic* dbg = dyn_cast<DbgVariableIntrinsic>(&inst)) {
        if (compat::get_alloca_for(dbg) == ai) {
          di_finder.processInstruction(*ai->getModule(), inst);
          return dbg->getVariable();
        }
      }
    }
    return llvm::None;
  };

  auto di_var = find_di_var(inst);
  p("\nValue", inst);
  p("Value Type", inst->getType());
  llvm::errs() << "Has meta: " << inst->hasMetadata() << "\n";
  // ai->getDebugLoc().dump();
  llvm::errs() << "DI type count: " << di_finder.type_count() << "\n";

  auto dbg_addrs = FindDbgAddrUses(inst);
  llvm::errs() << "Dbg addrs #" << dbg_addrs.size() << "\n";
  for (auto* dbg_ad : dbg_addrs) {
    llvm::errs() << *dbg_ad << "\n";
  }

  //  DenseMap<Value*, AllocaInst*> AllocaForValue;
  //  auto alloca_f = findAllocaForValue(inst, AllocaForValue);
  //  if (alloca_f) {
  //    llvm::errs() << *alloca_f << "\n";
  //  }
  if (di_var) {
    printer.traverseType(di_var.getValue()->getType());
    printer2.traverseType(di_var.getValue()->getType());
  }
}

void type_for(llvm::CallInst* call) {
  type_for_inst(llvm::dyn_cast<llvm::Instruction>(call));
}

void type_for(llvm::AllocaInst* ai) {
  using namespace llvm;

  DebugInfoFinder di_finder;
  //  di_finder.processModule(*ai->getModule());

  util::DISemPrinter printer(llvm::errs());
  util::DIPrinter printer2(llvm::errs());

  const auto p = [](StringRef name, auto node) {
    llvm::errs() << name << ": ";
    if (node == nullptr) {
      llvm::errs() << "<null>";
    } else {
      node->dump();
    }
  };

  const auto find_di_var = [&](auto* ai) -> llvm::Optional<DILocalVariable*> {
    auto& func = *ai->getFunction();
    for (auto& bb : func) {
      for (auto& inst : bb) {
        if (auto* dbg = dyn_cast<DbgVariableIntrinsic>(&inst)) {
          if (compat::get_alloca_for(dbg) == ai) {
            di_finder.processInstruction(*ai->getModule(), inst);
            return dbg->getVariable();
          }
        }
      }
    }
    return llvm::None;
  };

  //  p("\nValue", ai);
  //  p("Value Type", ai->getType());
  //  llvm::errs() << "Has meta: " << ai->hasMetadata() << "\n";
  //  // ai->getDebugLoc().dump();
  //
  auto di_var = find_di_var(ai);
  //  llvm::errs() << "DI type count: " << di_finder.type_count() << "\n";
  //  for (auto* types : di_finder.types()) {
  //    llvm::errs() << *types << "\n";
  //  }
  //
  //  auto dbg_addrs = FindDbgAddrUses(ai);
  //  llvm::errs() << "Dbg addrs #" << dbg_addrs.size() << "\n";
  //  for (auto* dbg_ad : dbg_addrs) {
  //    llvm::errs() << *dbg_ad << "\n";
  //  }

  if (di_var.hasValue()) {
    printer.traverseLocalVariable(di_var.getValue());
    printer2.traverseLocalVariable(di_var.getValue());
    parser::DITypeParser parser_types;
    parser_types.traverseLocalVariable(di_var.getValue());
    if (parser_types.hasCompound()) {
      std::string out_s;
      llvm::raw_string_ostream oss(out_s);
      io::emit(oss, parser_types.getAs<QualifiedCompound>().value());
      llvm::outs() << oss.str();
      {
        QualifiedCompound cmp;
        io::input(oss.str(), cmp);

        std::string out_s_2;
        llvm::raw_string_ostream oss_round(out_s_2);
        io::emit(oss_round, cmp);
        llvm::outs() << oss_round.str();
      }
    }
  }
}

llvm::Optional<llvm::DILocalVariable*> local_di_variable_for(llvm::AllocaInst* ai) {
  using namespace llvm;
  DebugInfoFinder di_finder;
  const auto find_di_var = [&](auto* ai) -> llvm::Optional<DILocalVariable*> {
    auto& func = *ai->getFunction();
    for (auto& inst : llvm::instructions(func)) {
      if (auto* dbg = dyn_cast<DbgVariableIntrinsic>(&inst)) {
        if (compat::get_alloca_for(dbg) == ai) {
          di_finder.processInstruction(*ai->getModule(), inst);
          return dbg->getVariable();
        }
      }
    }
    return llvm::None;
  };

  return find_di_var(ai);
}

}  // namespace dimeta
