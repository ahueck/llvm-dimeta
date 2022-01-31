//  Dimeta library
//  Copyright (c) 2022-2022 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//
#include "DimetaPass.h"

#include "llvm-c/Types.h"
#include "llvm/IR/LegacyPassManager.h"

using namespace llvm;

namespace dimeta {

class DimetaPass : public ModulePass {
 public:
  static char ID;  // NOLINT
  DimetaPass() : ModulePass(ID) {
  }

  bool runOnModule(Module& module) override {
    return true;
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
