//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "Dimeta.h"

#include "DIFinder.h"
#include "DIRootType.h"
#include "DITypeExtractor.h"
#include "DataflowAnalysis.h"
#include "DefUseAnalysis.h"
#include "DimetaParse.h"
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

namespace llvm {
class DbgVariableIntrinsic;
}  // namespace llvm

#if LLVM_VERSION_MAJOR == 10
// For FindDbgAddrUses:
#include "llvm/Transforms/Utils/Local.h"
#endif

namespace dimeta {

llvm::SmallVector<llvm::DIType*, 4> collect_types(llvm::ArrayRef<dataflow::ValuePath> paths_to_type) {
  using namespace llvm;
  SmallVector<llvm::DIType*, 4> di_types;
  llvm::transform(paths_to_type, dimeta::util::optional_back_inserter(di_types),
                  [](const auto& path) { return type::find_type(path); });
  return di_types;
}

auto final_ditype(std::optional<llvm::DIType*> root_ditype) -> std::pair<std::optional<llvm::DIType*>, int> {
  if (!root_ditype) {
    return {{}, 0};
  }
  int level{0};
  llvm::DIType* type = *root_ditype;
  while (llvm::isa<llvm::DIDerivedType>(type)) {
    auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
    if (ditype->getTag() == llvm::dwarf::DW_TAG_pointer_type) {
      ++level;
    }
    // void*-based derived types have basetype=null:
    if (ditype->getBaseType() == nullptr) {
      return {type, level};
    }
    type = ditype->getBaseType();
  }

  return {type, level};
};

std::optional<llvm::DIType*> type_for_malloclike(const llvm::CallBase* call) {
  const auto ditype_paths = dataflow::type_for_heap_call(call);

  const auto ditypes_vector = collect_types(ditype_paths);
  if (ditypes_vector.empty()) {
    return {};
  }
  return *ditypes_vector.begin();
}

std::optional<llvm::DIType*> type_for_newlike(const llvm::CallBase* call) {
  auto* heapalloc_md = call->getMetadata("heapallocsite");
  assert(heapalloc_md != nullptr && "Missing required heapallocsite metadata.");
  if (auto* type = llvm::dyn_cast<llvm::DIType>(heapalloc_md)) {
    //    util::DIPrinter printer(llvm::outs(), call->getParent()->getParent()->getParent());
    //    printer.traverseType(type);
    //    llvm::dbgs() << "Final Type: " << *type << "\n";
    return type;
  }
  return {};
}

std::optional<DimetaData> type_for(const llvm::CallBase* call) {
  using namespace llvm;
  const dimeta::memory::MemOps mem_ops;

  auto* cb_fun = call->getCalledFunction();
  if (!cb_fun) {
    return {};
  }

  if (!mem_ops.isAlloc(cb_fun->getName())) {
    LOG_TRACE("Skipping call base: " << cb_fun->getName());
    return {};
  }

  const auto is_cxx_new = mem_ops.isNewLike(cb_fun->getName());
  std::optional<llvm::DIType*> extracted_type{};

  int pointer_level_offset{0};
#ifdef DIMETA_USE_HEAPALLOCSITE
  if (is_cxx_new) {
    if (call->getMetadata("heapallocsite")) {
      LOG_TRACE("Type for new-like " << cb_fun->getName())
      extracted_type = type_for_newlike(call);
      // !heapallocsite gives the type after "new", i.e., new int -> int, new int*[n] -> int*.
      // Our malloc-related algorithm would return int* and int** respectively, however, hence:
      pointer_level_offset += 1;
    } else {
      LOG_DEBUG("new-like allocation does not have heapallocsite metadata.")
    }
  }
#endif

  if (!extracted_type) {
    LOG_TRACE("Type for malloc-like: " << cb_fun->getName())
    extracted_type = type_for_malloclike(call);
  }
  auto source_loc                        = difinder::find_location(call);
  const auto [final_type, pointer_level] = final_ditype(extracted_type);
  const auto meta = DimetaData{DimetaData::MemLoc::kHeap,           {}, extracted_type, final_type, source_loc,
                               pointer_level + pointer_level_offset};
  return meta;
}

std::optional<DimetaData> type_for(const llvm::AllocaInst* ai) {
  const auto local_di_var = difinder::find_local_variable(ai);

  if (local_di_var) {
    auto extracted_type                    = local_di_var.value()->getType();
    auto source_loc                        = difinder::find_location(ai);
    const auto [final_type, pointer_level] = final_ditype(extracted_type);
    const auto meta =
        DimetaData{DimetaData::MemLoc::kStack, local_di_var, extracted_type, final_type, source_loc, pointer_level};
    return meta;
  }

  return {};
}

std::optional<DimetaData> type_for(const llvm::GlobalVariable* gv) {
  llvm::SmallVector<llvm::DIGlobalVariableExpression*, 2> dbg_info;
  gv->getDebugInfo(dbg_info);
  if (!dbg_info.empty()) {
    auto gv_expr                           = *dbg_info.begin();
    auto gv_type                           = gv_expr->getVariable()->getType();
    const auto [final_type, pointer_level] = final_ditype(gv_type);
    return DimetaData{DimetaData::MemLoc::kGlobal, gv_expr->getVariable(), gv_type, final_type, {}, pointer_level};
  }
  return {};
}

std::optional<CompileUnitTypeList> compile_unit_types(const llvm::Module* module) {
  CompileUnitTypeList list;
  for (auto* compile_unit : module->debug_compile_units()) {
    CompileUnitTypes current_cu;
    current_cu.name = compile_unit->getFilename();
    for (auto* retained_type : compile_unit->getRetainedTypes()) {
      if (auto* type = llvm::dyn_cast<llvm::DIType>(retained_type)) {
        auto dimeta_result = parser::make_dimetadata(type);
        if (!dimeta_result) {
          continue;
        }
        current_cu.types.push_back(dimeta_result->type_);
      }
    }
    list.push_back(current_cu);
  }
  return (list.empty() ? std::optional<CompileUnitTypeList>{} : list);
}

}  // namespace dimeta
