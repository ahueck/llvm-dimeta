//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "support/Logger.h"
#include "type/DIVisitor.h"
#include "type/DIVisitorUtil.h"
#include "type/Dimeta.h"
#include "type/DimetaData.h"
#include "type/DimetaIO.h"
#include "type/DimetaParse.h"

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

#include <iomanip>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>

namespace llvm {
class PointerType;
}  // namespace llvm

using namespace llvm;

static cl::opt<bool> cl_dimeta_test_print_yaml("yaml", cl::init(true));
static cl::opt<bool> cl_dimeta_test_print_yaml_retained("yaml-retained", cl::init(false));
static cl::opt<bool> cl_dimeta_test_print_tree("dump-tree", cl::init(false));
static cl::opt<bool> cl_dimeta_test_stack_pointer("stack-pointer-skip", cl::init(false));
static cl::opt<bool> cl_dimeta_test_print("dump", cl::init(false));

namespace dimeta::test {

template <typename String>
inline std::string demangle(String&& s) {
  std::string name = std::string{s};
#if LLVM_VERSION_MAJOR == 17
  auto demangle = llvm::itaniumDemangle(name.data());
#else
  auto demangle = llvm::itaniumDemangle(name.data(), nullptr, nullptr, nullptr);
#endif
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

namespace util {

const std::string rep_string(std::string input, int rep) {
  std::ostringstream os;
  std::fill_n(std::ostream_iterator<std::string>(os), rep, input);
  return os.str();
};

auto to_string(dimeta::DimetaData& data, bool stack = false) {
  const std::string prefix = [&]() {
    switch (data.memory_location) {
      case DimetaData::MemLoc::kGlobal:
        return " Global";
      case DimetaData::MemLoc::kStack:
        return " Stack";
      default:
        return "";
    }
  }();

  std::string logging_message;
  llvm::raw_string_ostream rso(logging_message);
  rso << "Extracted Type" << prefix << ": " << log::ditype_str(data.entry_type.value_or(nullptr)) << "\n";
  rso << "Final Type" << prefix << ": " << log::ditype_str(data.base_type.value_or(nullptr)) << "\n";
  rso << "Pointer level: " << data.pointer_level << " (T" << rep_string("*", data.pointer_level) << ")\n";
  return rso.str();
};

auto print_loc(std::optional<location::SourceLocation> loc) {
  std::string logging_message;
  llvm::raw_string_ostream rso(logging_message);
  rso << "Location: ";
  if (loc) {
    rso << "\"" << loc->file << "\":\"" << loc->function << "\":" << loc->line;
  } else {
    rso << "empty";
  }
  return rso.str();
};

}  // namespace util

class TestPass : public ModulePass {
 private:
  Module* current_module{nullptr};
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
    this->current_module = &module;
    log::LogContext::get().setModule(&module);

    for (auto& global : module.globals()) {
      auto global_meta = type_for(&global);
      if (global_meta) {
        LOG_DEBUG("Type for global: " << global)
        LOG_DEBUG(util::to_string(global_meta.value()));
        auto located_type = located_type_for(&global);
        if (located_type) {
          LOG_DEBUG(util::print_loc(located_type->location));
        } else {
          LOG_ERROR("No located dimeta type for global")
        }
      }
    }

    auto compile_unit_list = dimeta::compile_unit_types(&module).value_or(CompileUnitTypeList{});
    if (cl_dimeta_test_print_yaml_retained) {
      std::string initial_oss_string;
      llvm::raw_string_ostream initial_oss(initial_oss_string);
      io::emit(initial_oss, compile_unit_list);
      llvm::outs() << initial_oss.str();
      // for (const auto& cu : compile_unit_list) {
      //   std::string initial_oss_string;
      //   llvm::raw_string_ostream initial_oss(initial_oss_string);
      //   io::emit(initial_oss, cu);
      //   llvm::outs() << initial_oss.str();
      // }
    }

    llvm::for_each(module.functions(), [&](auto& func) { return runOnFunc(func); });
    return false;
  }

  void runOnFunc(Function& func) {
    if (func.isDeclaration()) {
      return;
    }
    const auto f_name     = try_demangle(func);
    const auto f_name_ref = llvm::StringRef(f_name);
    if (f_name_ref.startswith("std::") || f_name_ref.startswith("__")) {
      return;
    }

    LOG_MSG("\nFunction: " << f_name << ":");

    const auto get_located_type = [&](auto* call_inst) -> std::optional<LocatedType> {
      auto located_type = located_type_for(call_inst);
      if (located_type) {
        LOG_DEBUG(util::print_loc(located_type->location));
        return located_type;
      }
      LOG_ERROR("No located dimeta type.")
      return {};
    };

    const auto serialize_yaml = [&](auto* inst) {
      auto located_type = get_located_type(inst);
      bool result{false};
      if (located_type) {
        result = serialization_roundtrip(located_type.value(), cl_dimeta_test_print_yaml.getValue());
      }
      LOG_MSG(*inst << ": Yaml Verifier: " << static_cast<int>(result));
    };

    const auto print_di_tree = [&](const DimetaData& di_var) {
      if (cl_dimeta_test_print) {
        visitor::util::print_dinode(std::get<DILocalVariable*>(di_var.di_variable.value()), outs(), current_module);
      }
    };

    const auto dump_di_tree = [&](const DimetaData& di_var) {
      if (cl_dimeta_test_print_tree) {
        auto local_di_var = std::get<DILocalVariable*>(di_var.di_variable.value());
#if LLVM_MAJOR_VERSION < 14
        local_di_var->print(outs(), current_module);
#else
        local_di_var->dumpTree(current_module);
#endif
      }
    };

    for (auto& inst : llvm::instructions(func)) {
      if (auto* call_inst = dyn_cast<CallBase>(&inst)) {
        auto ditype_meta = type_for(call_inst);
        if (ditype_meta) {
          LOG_DEBUG("Type for heap-like: " << *call_inst)
          LOG_DEBUG(util::to_string(ditype_meta.value()) << "\n");
          // auto result = located_type_for(ditype_meta.value());
          serialize_yaml(call_inst);
          continue;
        }
        get_located_type(call_inst);
      }

      if (auto* alloca_inst = dyn_cast<AllocaInst>(&inst)) {
        if (cl_dimeta_test_stack_pointer && isa<llvm::PointerType>(alloca_inst->getAllocatedType())) {
          LOG_DEBUG("Skip " << *alloca_inst << "\n");
          continue;
        }
        auto di_var = type_for(alloca_inst);
        if (di_var) {
          LOG_DEBUG("Type for alloca: " << *alloca_inst)
          LOG_DEBUG(util::to_string(di_var.value()) << "\n");
          dump_di_tree(di_var.value());
          print_di_tree(di_var.value());
          serialize_yaml(alloca_inst);
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
