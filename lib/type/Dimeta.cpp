//  Dimeta library
//  Copyright (c) 2022-2022 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "Dimeta.h"

#include "DIVisitor.h"

#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Support/raw_ostream.h"

namespace dimeta {

namespace compat {
template <typename DbgVar>
llvm::Value* get_alloca_for(const DbgVar* dbg_var) {
#if LLVM_VERSION_MAJOR < 14
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
    for (auto& bb : func) {
      for (auto& inst : bb) {
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
    }
    return llvm::None;
  };

  p("\nValue", inst);
  p("Value Type", inst->getType());
  llvm::errs() << "Has meta: " << inst->hasMetadata() << "\n";
  // ai->getDebugLoc().dump();

  auto di_var = find_di_var(inst);
  if (di_var) {
    printer.visit(di_var.getValue()->getType());
    printer2.visit(di_var.getValue()->getType());
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
        if (DbgVariableIntrinsic* dbg = dyn_cast<DbgVariableIntrinsic>(&inst)) {
          if (compat::get_alloca_for(dbg) == ai) {
            di_finder.processInstruction(*ai->getModule(), inst);
            return dbg->getVariable();
          }
        }
      }
    }
    return llvm::None;
  };

  p("\nValue", ai);
  p("Value Type", ai->getType());
  llvm::errs() << "Has meta: " << ai->hasMetadata() << "\n";
  // ai->getDebugLoc().dump();

  auto di_var = find_di_var(ai);
  if (di_var) {
    printer.visit(di_var.getValue());
    printer2.visit(di_var.getValue());
  }

  //  auto& O = llvm::errs();
  //  O << "\n------------------\n";
  //  for (const DIType* T : di_finder.types()) {
  //    printer.visit(T);
  //    O << "\n";
  //  }
  //  O << "\n------------------\n";

  //  if (di_var) {
  //    auto* v = *di_var.getPointer();
  //    p("DI_LocalVar", v);
  //    auto di_type = v->getType();
  //    p("DI_Type", di_type);
  //
  //    if (auto der_type = dyn_cast<DIDerivedType>(di_type)) {
  //      p("DI_baseType", der_type->getBaseType());
  //      llvm::errs() << "DI_deriv name: " << der_type->getName() << "\n......\n";
  //      llvm::errs() << "DI_basetype name: " << der_type->getBaseType()->getName() << "\n......\n";
  //    }
  //
  //    if (auto com_type = dyn_cast<DICompositeType>(di_type)) {
  //      p("baseType", com_type->getBaseType());
  //      llvm::errs() << "name: " << com_type->getName() << "\n......\n";
  //      for (auto elems : com_type->getElements()) {
  //        p("Elem", elems);
  //      }
  //    }
  //  } else {
  //    p("Done", ai);
  //  }
}

}  // namespace dimeta
