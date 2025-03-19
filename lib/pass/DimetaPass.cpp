//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//
#include "DimetaPass.h"

#include "Dimeta.h"

#include "llvm-c/Types.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/ilist_iterator.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace dimeta {

class TestPass : public PassInfoMixin<TestPass> {
 public:
  PreservedAnalyses run(Module& M, ModuleAnalysisManager&) {
    llvm::for_each(M.functions(), [&](auto& func) { return runOnFunc(func); });
    PreservedAnalyses pa = PreservedAnalyses::all();
    return pa;
  }

  static void runOnFunc(Function& func) {
    if (func.isDeclaration()) {
      return;
    }

    llvm::outs() << "Function: " << func.getName() << ":\n";
    llvm::outs() << "-------------------------------------\n";

    const auto ditype_tostring = [](auto* ditype) {
      llvm::DIType* type = ditype;
      while (llvm::isa<llvm::DIDerivedType>(type)) {
        auto derived = llvm::dyn_cast<llvm::DIDerivedType>(type);
        // void*-based derived types:
        if (derived->getBaseType() == nullptr) {
          return type;
        }
        type = derived->getBaseType();
      }

      return type;
    };

    for (auto& inst : llvm::instructions(func)) {
      if (auto* call_inst = dyn_cast<CallBase>(&inst)) {
        auto ditype = type_for(call_inst);
        if (ditype) {
          llvm::outs() << "Final heap type: " << *ditype_tostring(ditype.value().base_type.value()) << "\n\n";
        }
      }
      if (auto* ai = dyn_cast<AllocaInst>(&inst)) {
        auto alloca_di = dimeta::type_for(ai);
        if (alloca_di) {
          llvm::outs() << "Final alloca type: " << *ditype_tostring(alloca_di.value().base_type.value()) << "\n\n";
        }
      }
      //      if (auto* call = dyn_cast<CallInst>(&inst)) {
      //        if (call->getIntrinsicID() == llvm::Intrinsic::not_intrinsic) {
      //          dimeta::type_for(call);
      //        }
      //      }
    }
    llvm::outs() << "-------------------------------------\n";
  }
};

}  // namespace dimeta

#define DEBUG_TYPE "dimeta-analysis-pass"

PassPluginLibraryInfo getPassPluginInfo() {
  const auto callback = [](PassBuilder& PB) {
#if LLVM_VERSION_MAJOR < 12
    PB.registerPipelineStartEPCallback([&](ModulePassManager& MPM) {
#else
    PB.registerPipelineEarlySimplificationEPCallback([&](ModulePassManager& MPM, auto) {
#endif
      MPM.addPass(dimeta::TestPass());
      return true;
    });
  };

  return {LLVM_PLUGIN_API_VERSION, DEBUG_TYPE, "0.0.1", callback};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getPassPluginInfo();
}