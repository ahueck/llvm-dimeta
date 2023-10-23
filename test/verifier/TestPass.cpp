//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIVisitor.h"
#include "Dimeta.h"
#include "DimetaData.h"
#include "MetaIO.h"
#include "MetaParse.h"
#include "support/Logger.h"
#include "type/Dimeta.h"

#include "llvm-c/Types.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/ilist_iterator.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <iterator>
#include <optional>
#include <sstream>
#include <string>

namespace llvm {
class PointerType;

class PointerType;
}  // namespace llvm

using namespace llvm;

static cl::opt<bool> cl_dimeta_test_print_yaml("yaml", cl::init(false));
static cl::opt<bool> cl_dimeta_test_print_tree("dump-tree", cl::init(false));
static cl::opt<bool> cl_dimeta_test_print("dump", cl::init(false));

namespace dimeta::test {

template <typename String>
inline std::string demangle(String&& s) {
  std::string name = std::string{s};
  auto demangle    = llvm::itaniumDemangle(name.data(), nullptr, nullptr, nullptr);
  if (demangle && !std::string(demangle).empty()) {
    return {demangle};
  }
  return name;
}

template <typename T>
inline std::string try_demangle(const T& site) {
  if constexpr (std::is_same_v<T, llvm::CallBase>) {
    if (site.isIndirectCall()) {
      return "";
    }
    return demangle(site.getCalledFunction()->getName());
  } else {
    if constexpr (std::is_same_v<T, llvm::Function>) {
      return demangle(site.getName());
    } else {
      return demangle(site);
    }
  }
}

class TestPass : public ModulePass {
 private:
  template <typename Type>
  bool serialization_roundtrip(Type compound, bool print_yaml = false) {
    std::string initial_oss_string;
    llvm::raw_string_ostream initial_oss(initial_oss_string);
    io::emit(initial_oss, compound);

    if (print_yaml) {
      llvm::outs() << initial_oss.str();
    }

    Type cmp;
    dimeta::io::input(initial_oss.str(), cmp);

    std::string roundtrip_oss_string;
    llvm::raw_string_ostream roundtrip_oss(roundtrip_oss_string);
    io::emit(roundtrip_oss, cmp);

    llvm::StringRef const initial{initial_oss.str()};

    return initial.compare(llvm::StringRef{roundtrip_oss.str()}) == 0;
  }

 public:
  static char ID;  // NOLINT
  TestPass() : ModulePass(ID) {
  }

  bool runOnModule(Module& module) override {
    log::LogContext::get().setModule(&module);
    llvm::for_each(module.functions(), [&](auto& func) { return runOnFunc(func); });
    return false;
  }

  void runOnFunc(Function& func) {
    if (func.isDeclaration()) {
      return;
    }
    const auto f_name     = try_demangle(func);
    const auto f_name_ref = llvm::StringRef(f_name);
    if (f_name_ref.contains("std::") || f_name_ref.startswith("__")) {
      return;
    }

    LOG_MSG("Function: " << func.getName() << ":");

    //    const auto ditype_tostring = [](auto* ditype) {
    //      llvm::DIType* type = ditype;
    //      while (llvm::isa<llvm::DIDerivedType>(type)) {
    //        auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
    //        // void*-based derived types:
    //        if (ditype->getBaseType() == nullptr) {
    //          return log::ditype_str(type);
    //        }
    //        type = ditype->getBaseType();
    //      }
    //
    //      return log::ditype_str(type);
    //    };

    const auto rep_string = [](auto& input, auto rep) {
      std::ostringstream os;
      std::fill_n(std::ostream_iterator<std::string>(os), rep, input);
      return os.str();
    };

    const auto to_string = [&rep_string](dimeta::DimetaData& data, bool stack = false) {
      const std::string prefix = stack ? " Stack" : "";
      std::string logging_message;
      llvm::raw_string_ostream rso(logging_message);
      rso << "Extracted Type" << prefix << ": " << log::ditype_str(data.entry_type.value()) << "\n";
      rso << "Final Type" << prefix << ": " << log::ditype_str(data.base_type.value()) << "\n";
      rso << "Pointer level: " << data.pointer_level << " (T" << rep_string("*", data.pointer_level) << ")";
      return rso.str();
    };

    for (auto& inst : llvm::instructions(func)) {
      if (auto* call_inst = dyn_cast<CallBase>(&inst)) {
        auto ditype_meta = type_for(call_inst);
        if (ditype_meta) {
          LOG_DEBUG("Type for heap-like: " << *call_inst)
          //          LOG_DEBUG("Extracted Type: " << log::ditype_str(ditype_meta.value()) << "\n");
          //          LOG_MSG("Final Type: " << ditype_tostring(ditype_meta.value()) << "\n");
          LOG_DEBUG(to_string(ditype_meta.value()));
        }
      }

      if (auto* alloca_inst = dyn_cast<AllocaInst>(&inst)) {
        if (isa<llvm::PointerType>(alloca_inst->getAllocatedType())) {
          LOG_DEBUG("Skip " << *alloca_inst);
          continue;
        }
        auto di_var = type_for(alloca_inst);
        if (di_var) {
          LOG_DEBUG("Type for alloca: " << *alloca_inst)
          //          LOG_MSG("Final Stack Type: " << ditype_tostring(di_var.value()->getType()) << "\n");
          LOG_DEBUG(to_string(di_var.value(), true));
        }
        if (di_var) {
          parser::DITypeParser parser_types;
          parser_types.traverseLocalVariable(di_var.value().stack_alloca.value());

          if (cl_dimeta_test_print_tree) {
#if LLVM_MAJOR_VERSION < 14
            di_var.value().stack_alloca.value()->print(llvm::outs(), func.getParent());
#else
            di_var.stack_alloca.value()->dumpTree(func.getParent());
#endif
          }

          if (cl_dimeta_test_print) {
            util::DIPrinter printer(llvm::outs(), func.getParent());
            printer.traverseLocalVariable(di_var.value().stack_alloca.value());
          }

          bool result{false};
          if (parser_types.hasCompound()) {
            auto const qual_type = parser_types.getAs<QualifiedCompound>().value();
            result               = serialization_roundtrip(qual_type, cl_dimeta_test_print_yaml.getValue());
          } else if (parser_types.hasFundamental()) {
            auto const qual_type = parser_types.getAs<QualifiedFundamental>().value();
            result               = serialization_roundtrip(qual_type, cl_dimeta_test_print_yaml.getValue());
          }
          LOG_MSG(*alloca_inst << ": Yaml Verifier: " << static_cast<int>(result));
        }
      }
    }
  }

  ~TestPass() override = default;
};

}  // namespace dimeta::test

#define DEBUG_TYPE "dimeta-test-pass"

char dimeta::test::TestPass::ID = 0;  // NOLINT

static RegisterPass<dimeta::test::TestPass> x("dimeta-test", "Dimeta Data Type Test Pass");  // NOLINT

ModulePass* createTestPass() {
  return new dimeta::test::TestPass();
}

extern "C" void AddTestPass(LLVMPassManagerRef pass_manager) {
  unwrap(pass_manager)->add(createTestPass());
}
