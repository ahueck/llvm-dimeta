//  Dimeta library
//  Copyright (c) 2022-2022 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//
#include "DimetaPass.h"

#include "Dimeta.h"

#include "llvm-c/Types.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace dimeta {

class DimetaPass : public ModulePass {
 public:
  static char ID;  // NOLINT
  DimetaPass() : ModulePass(ID) {
  }

  bool runOnModule(Module& module) override {
    llvm::for_each(module.functions(), [&](auto& func) { return runOnFunc(func); });
    return false;
  }

  static void runOnFunc(Function& func) {
    if (func.isDeclaration()) {
      return;
    }

    llvm::outs() << "Function: " << func.getName() << ":\n";
    llvm::outs() << "-------------------------------------\n";

    for (auto& bblock : func) {
      for (auto& inst : bblock) {
        if (auto* ai = dyn_cast<AllocaInst>(&inst)) {
          dimeta::type_for(ai);
        }
        if (auto* call = dyn_cast<CallInst>(&inst)) {
          if (call->getIntrinsicID() == llvm::Intrinsic::not_intrinsic) {
            dimeta::type_for(call);
          }
        }
      }
    }
    llvm::outs() << "-------------------------------------\n";
  }

  ~DimetaPass() override = default;
};

}  // namespace dimeta

#define DEBUG_TYPE "dimeta-analysis-pass"

char dimeta::DimetaPass::ID = 0;  // NOLINT

static RegisterPass<dimeta::DimetaPass> x("dimeta", "Dimeta Pass");  // NOLINT

ModulePass* createDimetaPass() {
  return new dimeta::DimetaPass();
}

extern "C" void AddDimetaPass(LLVMPassManagerRef pass_manager) {
  unwrap(pass_manager)->add(createDimetaPass());
}
