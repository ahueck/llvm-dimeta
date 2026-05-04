//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "Dimeta.h"

#include "DIFinder.h"
#include "DIRootType.h"
#include "DITypeExtractor.h"
#include "DIUtil.h"
#include "DataflowAnalysis.h"
#include "DefUseAnalysis.h"
#include "DimetaData.h"
#include "DimetaParse.h"
#include "MemoryOps.h"
#include "Util.h"
#include "ValuePath.h"
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
#include <optional>
#include <string>

namespace llvm {
class DbgVariableIntrinsic;
}  // namespace llvm

#if LLVM_VERSION_MAJOR == 10
// For FindDbgAddrUses:
#include "llvm/Transforms/Utils/Local.h"
#endif

namespace dimeta {

namespace fortran {
struct FortranType {
  llvm::DIType* type{nullptr};
  std::optional<ShapeData> shape_argument;
};
std::optional<FortranType> di_type_for(const llvm::Value* value, const llvm::CallBase* call = nullptr);
}  // namespace fortran

namespace experimental {
std::optional<llvm::DIType*> di_type_for(const llvm::Value* value);
}

std::optional<llvm::DIType*> type_for_malloclike(const llvm::CallBase* call);
std::optional<llvm::DIType*> type_for_newlike(const llvm::CallBase* call);
auto final_ditype(std::optional<llvm::DIType*> root_ditype) -> std::pair<std::optional<llvm::DIType*>, int>;

namespace {

enum class CallBaseTypeSource {
  kReturnValueForwardThenBackward,
  kArgumentBackward,
  kFortranDescriptor,
  kHeapAllocSite,
};

struct ResolvedCallBaseTypeConfig {
  CallBaseTypeSource source;
  unsigned argument_index{0};
  int pointer_level_offset{0};
};

std::optional<ResolvedCallBaseTypeConfig> resolve_auto_callbase_config(const llvm::CallBase* call) {
  if (call == nullptr) {
    return {};
  }

  auto* cb_fun = call->getCalledFunction();
  if (cb_fun == nullptr) {
    return {};
  }

  const dimeta::memory::MemOps mem_ops;
  if (!mem_ops.isAlloc(cb_fun->getName())) {
    LOG_TRACE("Skipping call base: " << cb_fun->getName());
    return {};
  }

  if (mem_ops.isFortranLike(cb_fun->getName())) {
    return ResolvedCallBaseTypeConfig{CallBaseTypeSource::kFortranDescriptor, 0, 0};
  }

  if (mem_ops.isCudaLike(cb_fun->getName())) {
    return ResolvedCallBaseTypeConfig{CallBaseTypeSource::kArgumentBackward, 0, 0};
  }

  if (mem_ops.isMpiLike(cb_fun->getName())) {
    return ResolvedCallBaseTypeConfig{CallBaseTypeSource::kArgumentBackward, 2, 0};
  }

#ifdef DIMETA_USE_HEAPALLOCSITE
  if (mem_ops.isNewLike(cb_fun->getName()) && call->getMetadata("heapallocsite") != nullptr) {
    return ResolvedCallBaseTypeConfig{CallBaseTypeSource::kHeapAllocSite, 0, 1};
  }
#endif

  return ResolvedCallBaseTypeConfig{CallBaseTypeSource::kReturnValueForwardThenBackward, 0, 0};
}

std::optional<ResolvedCallBaseTypeConfig> resolve_callbase_config(const llvm::CallBase* call,
                                                                  const CallBaseTypeConfig& config) {
  if (call == nullptr) {
    return {};
  }

  if (config.dataflow == CallBaseTypeConfig::Dataflow::kAuto) {
    return resolve_auto_callbase_config(call);
  }

  if (config.dataflow == CallBaseTypeConfig::Dataflow::kArgumentBackward) {
    if (config.argument_index >= call->arg_size()) {
      LOG_DEBUG("Invalid argument index for callbase config: " << config.argument_index);
      return {};
    }
    return ResolvedCallBaseTypeConfig{CallBaseTypeSource::kArgumentBackward, config.argument_index,
                                      config.pointer_level_offset};
  }

  if (config.dataflow == CallBaseTypeConfig::Dataflow::kReturnValueForwardThenBackward) {
    return ResolvedCallBaseTypeConfig{CallBaseTypeSource::kReturnValueForwardThenBackward, 0,
                                      config.pointer_level_offset};
  }

  return {};
}

std::optional<DimetaData> type_for_resolved_callbase(const llvm::CallBase* call,
                                                     const ResolvedCallBaseTypeConfig& config) {
  if (call == nullptr) {
    return {};
  }

  std::optional<llvm::DIType*> extracted_type{};
  std::optional<ShapeData> shape_type{};
  int pointer_level_offset{config.pointer_level_offset};

  switch (config.source) {
    case CallBaseTypeSource::kArgumentBackward: {
      extracted_type = experimental::di_type_for(call->getArgOperand(config.argument_index));
      break;
    }
    case CallBaseTypeSource::kReturnValueForwardThenBackward: {
      extracted_type = type_for_malloclike(call);
      break;
    }
    case CallBaseTypeSource::kFortranDescriptor: {
      if (call->arg_size() == 0) {
        return {};
      }
      auto fortran_type = fortran::di_type_for(call->getArgOperand(0), call);
      if (fortran_type) {
        extracted_type = fortran_type->type;
        if (fortran_type->shape_argument) {
          shape_type = fortran_type->shape_argument;
        }
      }
      break;
    }
    case CallBaseTypeSource::kHeapAllocSite: {
      extracted_type = type_for_newlike(call);
      break;
    }
  }

  auto source_loc                        = difinder::find_location(call);
  const auto [final_type, pointer_level] = final_ditype(extracted_type);

  return DimetaData{DimetaData::MemLoc::kHeap,           {}, extracted_type, final_type, source_loc, shape_type,
                    pointer_level + pointer_level_offset};
}

}  // namespace

llvm::SmallVector<llvm::DIType*, 4> collect_types(const llvm::CallBase* call,
                                                  llvm::ArrayRef<dataflow::ValuePath> paths_to_type) {
  using namespace llvm;
  SmallVector<llvm::DIType*, 4> di_types;
  llvm::transform(paths_to_type, dimeta::util::optional_back_inserter(di_types),
                  [&](const auto& path) { return type::find_type(dataflow::CallValuePath{call, path}); });
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
    if (di::util::is_pointer(*ditype, false)) {
      ++level;
    }
    // void*-based derived types have basetype=null:
    if (ditype->getBaseType() == nullptr) {
      return {type, level};
    }
    type = ditype->getBaseType();
  }

  return {type, level};
}

std::optional<llvm::DIType*> type_for_malloclike(const llvm::CallBase* call) {
  auto local = difinder::get_array_access_assignment(call);
  if (local) {
    LOG_DEBUG("Call has local variable " << *call)
    // LOG_DEBUG("Call type " << log::ditype_str(local.value()->getType()))
    auto base_type = local.value().var->getType();
    if (local.value().array_access) {
      if (auto* array_type = llvm::dyn_cast<llvm::DICompositeType>(base_type)) {
        LOG_DEBUG("Returning type of access to array " << log::ditype_str(array_type))
        return array_type->getBaseType();
      }
    }
    return base_type;
  }

  const auto ditype_paths = dataflow::type_for_heap_call(call);

  LOG_DEBUG("Found paths, now collecting types")
  const auto ditypes_vector = collect_types(call, ditype_paths);
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

std::optional<DimetaData> type_for(const llvm::CallBase* call, const CallBaseTypeConfig& config) {
  auto resolved = resolve_callbase_config(call, config);
  if (!resolved) {
    if (config.dataflow == CallBaseTypeConfig::Dataflow::kAuto) {
      LOG_TRACE("Skipping call base in auto mode.");
    } else {
      LOG_DEBUG("Could not resolve explicit callbase type configuration.");
    }
    return {};
  }

  auto result = type_for_resolved_callbase(call, *resolved);
  if (!result) {
    return {};
  }

  if (config.dataflow != CallBaseTypeConfig::Dataflow::kAuto && !result->entry_type) {
    LOG_DEBUG("Explicit callbase type extraction failed.");
    return {};
  }

  return result;
}

std::optional<DimetaData> type_for(const llvm::AllocaInst* ai) {
  const auto local_di_var = difinder::find_local_variable(ai);

  const auto passed = dataflow::fortran::passed_to_fortran_helper(ai);
  if (passed) {
    LOG_DEBUG("Skip allocation passed to Flang intrinsic")
    return {};
  }

  if (local_di_var) {
    auto extracted_type                    = local_di_var.value()->getType();
    auto source_loc                        = difinder::find_location(ai);
    const auto [final_type, pointer_level] = final_ditype(extracted_type);
    const auto meta =
        DimetaData{DimetaData::MemLoc::kStack, local_di_var, extracted_type, final_type, source_loc, {}, pointer_level};
    return meta;
  }

  LOG_DEBUG("No local_variable for " << *ai)

  return {};
}

std::optional<DimetaData> type_for(const llvm::GlobalVariable* gv) {
  llvm::SmallVector<llvm::DIGlobalVariableExpression*, 2> dbg_info;
  gv->getDebugInfo(dbg_info);
  if (!dbg_info.empty()) {
    auto gv_expr                           = *dbg_info.begin();
    auto gv_type                           = gv_expr->getVariable()->getType();
    const auto [final_type, pointer_level] = final_ditype(gv_type);
    return DimetaData{DimetaData::MemLoc::kGlobal, gv_expr->getVariable(), gv_type, final_type, {}, {}, pointer_level};
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

namespace fortran {
std::optional<FortranType> di_type_for(const llvm::Value* value, const llvm::CallBase* call) {
  assert(value != nullptr);
  auto shape = dataflow::fortran::shape_from_value(value);

  auto paths                = dataflow::experimental::path_from_value(value);
  const auto ditypes_vector = collect_types(call, paths);
  if (ditypes_vector.empty()) {
    return {};
  }

  return FortranType{*ditypes_vector.begin(), shape};
}
}  // namespace fortran

namespace experimental {
std::optional<llvm::DIType*> di_type_for(const llvm::Value* value) {
  auto paths                = dataflow::experimental::path_from_value(value);
  const auto ditypes_vector = collect_types(nullptr, paths);
  if (ditypes_vector.empty()) {
    return {};
  }

  return *ditypes_vector.begin();
}

std::optional<QualifiedType> type_for(const llvm::Value* value) {
  auto paths                = dataflow::experimental::path_from_value(value);
  const auto ditypes_vector = collect_types(nullptr, paths);
  if (ditypes_vector.empty()) {
    return {};
  }

  for (const auto& type : ditypes_vector) {
    auto dimeta_result = parser::make_dimetadata(type);
    if (!dimeta_result) {
      continue;
    }
    return dimeta_result->type_;
  }

  return {};
}

}  // namespace experimental

}  // namespace dimeta
