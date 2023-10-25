//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "Dimeta.h"

#include "DataflowAnalysis.h"
#include "DefUseAnalysis.h"
#include "GEP.h"
#include "MemoryOps.h"
#include "TBAA.h"
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

namespace ditype {

namespace util {
bool contains_gep(const dataflow::ValuePath& path) {
  return llvm::find_if(path.path_to_value, [](const auto* val) { return llvm::isa<llvm::GetElementPtrInst>(val); }) !=
         std::end(path.path_to_value);
}
}  // namespace util

std::optional<llvm::DILocalVariable*> find_local_var_for(const llvm::AllocaInst* ai) {
  using namespace llvm;
  //  DebugInfoFinder di_finder;
  const auto find_di_var = [&](auto* ai) -> std::optional<DILocalVariable*> {
    auto& func = *ai->getFunction();
    for (auto& inst : llvm::instructions(func)) {
      if (auto* dbg = dyn_cast<DbgVariableIntrinsic>(&inst)) {
        if (compat::get_alloca_for(dbg) == ai) {
          //          di_finder.processInstruction(*ai->getModule(), inst);
          return dbg->getVariable();
        }
      }
    }
    return {};
  };
  return find_di_var(ai);
}

std::optional<llvm::DIType*> find_type_root(const dataflow::ValuePath& path) {
  using namespace llvm;
  const auto* root_value = path.value();

  if (const auto* ret = dyn_cast<ReturnInst>(root_value)) {
    auto* sub_prog  = ret->getFunction()->getSubprogram();
    auto type_array = sub_prog->getType()->getTypeArray();
    if (type_array.size() > 0) {
      return {*type_array.begin()};
    }
    return {};
  }

  if (const auto* alloca = dyn_cast<AllocaInst>(root_value)) {
    auto local_di_var = find_local_var_for(alloca);
    if (local_di_var) {
      return local_di_var.value()->getType();
    }
    return {};
  }

  if (const auto* call_inst = llvm::dyn_cast<CallBase>(root_value)) {
    const auto* called_f = call_inst->getCalledFunction();
    if (called_f != nullptr) {
      // Argument passed to current call:
      const auto* arg_val = path.previous_value();
      assert(arg_val != nullptr && "Previous value should be argument to some function!");
      // Argument number:
      const auto* const arg_pos = llvm::find_if(
          call_inst->args(), [&arg_val](const auto& arg_use) -> bool { return arg_use.get() == arg_val; });
      auto arg_num = std::distance(call_inst->arg_begin(), arg_pos);
      // Extract debug info from function at arg_num:
      if (auto* sub_program = called_f->getSubprogram(); sub_program != nullptr) {
        const auto sub_prog_arg_pos = arg_num + 1;
        auto types_of_subprog       = sub_program->getType()->getTypeArray();
        assert((types_of_subprog.size() > sub_prog_arg_pos) && "Type array smaller than arg num!");
        auto* type = types_of_subprog[sub_prog_arg_pos];
        return type;
      }
    }
    return {};
  }

  if (const auto* global_variable = llvm::dyn_cast<llvm::GlobalVariable>(root_value)) {
    auto dbg_md = global_variable->getMetadata("dbg");
    if (!dbg_md) {
      return {};
    }
    if (auto* global_expression = llvm::dyn_cast<llvm::DIGlobalVariableExpression>(dbg_md)) {
      return global_expression->getVariable()->getType();
    }
    return {};
  }

  if (const auto* argument = llvm::dyn_cast<llvm::Argument>(root_value)) {
    if (auto* subprogram = argument->getParent()->getSubprogram(); subprogram != nullptr) {
      const auto arg_pos    = argument->getArgNo() + 1;
      const auto type_array = subprogram->getType()->getTypeArray();
      assert(arg_pos < type_array.size() && "Arg position greater than DI type array of subprogram!");
      return type_array[arg_pos];
    }

    return {};
  }

  if (const auto* const_expr = llvm::dyn_cast<llvm::ConstantExpr>(root_value)) {
    LOG_DEBUG("ConstantExpr unsupported");
  }

  LOG_DEBUG("No matching value found for " << *root_value);
  return {};
}

template <typename Iter, typename Iter2>
std::optional<llvm::DIType*> reset_ditype(llvm::DIType* type_to_reset, const Iter& path_iter, const Iter2& path_end) {
  std::optional<llvm::DIType*> type = type_to_reset;
  if (type) {
    LOG_DEBUG("Type to reset: " << log::ditype_str(*type));
  }

  // Look at next value after gep [path_iter] (e.g., a load or the final store)
  auto next_value = std::next(path_iter);
  if (next_value == path_end) {
    return {};
  }

  // Re-set the DIType from the gep, if presence of:
  // - a load ofter a gep is likely the first element of the composite type
  // - a load also resolves to the basetype w.r.t. an array composite
  // - a store with a ditype(array) is likely the first element of the array
  LOG_DEBUG("Looking at " << **next_value);
  if (llvm::isa<llvm::LoadInst>(*next_value)) {
    // workaround for gep/array_composite_sub.c (non-optim/optim):
    auto next_after_load = std::next(next_value);
    assert(next_after_load != path_end && "After load there should be a instruction!");
    if (!llvm::isa<llvm::StoreInst>(*next_after_load)) {
      // a load resolves a pointer level in the DI type, but we only look at the final gep before the store.
      // for now..
      return {};
    }

    auto ditype_val = type.value();
    LOG_DEBUG("  with ditype " << log::ditype_str(ditype_val));
    if (auto* ptr_to_type = llvm::dyn_cast<llvm::DIDerivedType>(ditype_val)) {
      auto base_type = ptr_to_type->getBaseType();
      assert(base_type != nullptr && "Pointer points to null-type (void*?)");

      if (auto* composite = llvm::dyn_cast<llvm::DICompositeType>(base_type)) {
        assert(!composite->getElements().empty() && "Load should target member of composite type!");

        auto first_elem = composite->getElements()[0];
        if (auto loaded_elem = llvm::dyn_cast<llvm::DIType>(first_elem)) {
          LOG_DEBUG("Loaded from extracted type: " << log::ditype_str(loaded_elem))
          if (loaded_elem->getTag() == llvm::dwarf::DW_TAG_member) {
            loaded_elem = llvm::cast<llvm::DIDerivedType>(loaded_elem)->getBaseType();
            LOG_DEBUG("Resetting loaded element from member to base " << log::ditype_str(loaded_elem))
          }
          type = loaded_elem;
        }
      }
      if (auto* ptr_to_ptr = llvm::dyn_cast<llvm::DIDerivedType>(base_type)) {
        if (ptr_to_ptr->getTag() == llvm::dwarf::DW_TAG_pointer_type) {
          type = ptr_to_ptr;
        }
      }
    }
    if (auto* ptr_to_array = llvm::dyn_cast<llvm::DICompositeType>(ditype_val)) {
      if (ptr_to_array->getTag() == llvm::dwarf::DW_TAG_array_type) {
        LOG_DEBUG("Loaded from extracted type of array type " << log::ditype_str(ptr_to_array->getBaseType()))
        type = ptr_to_array->getBaseType();
      }
    }
  }

  if (auto store_inst = llvm::dyn_cast<llvm::StoreInst>(*next_value)) {
    auto ditype_val = type.value();
    LOG_DEBUG("With stored to ditype " << log::ditype_str(ditype_val));
    if (auto* array_to_composite = llvm::dyn_cast<llvm::DICompositeType>(ditype_val)) {
      if (array_to_composite->getTag() == llvm::dwarf::DW_TAG_array_type) {
        LOG_DEBUG("Loaded from extracted type of array type " << log::ditype_str(array_to_composite->getBaseType()))
        type = array_to_composite->getBaseType();
      }
    }
    // A store directly to a pointer, remove one level of "pointerness", see test heap_matrix_simple with -O2.
    if (auto* ptr_type = llvm::dyn_cast<llvm::DIDerivedType>(ditype_val)) {
      assert(ptr_type->getTag() == llvm::dwarf::DW_TAG_pointer_type && "Expected a store inst to a pointer here.");
      auto base_type = ptr_type->getBaseType();
      if (llvm::isa<llvm::Argument>(store_inst->getPointerOperand())) {
        // alloca vs. argument: argument has no indirection for store, hence, we can substract a pointer-level
        if (auto* ptr_to_ptr = llvm::dyn_cast<llvm::DIDerivedType>(base_type)) {
          if (ptr_to_ptr->getTag() == llvm::dwarf::DW_TAG_pointer_type) {
            LOG_DEBUG("Store to a pointer type, resolving to " << log::ditype_str(base_type))
            return base_type;
          }
        }
      }
    }
  }

  return type;
}

std::optional<llvm::DIType*> find_type(const dataflow::ValuePath& path) {
  auto type = find_type_root(path);

  bool has_gep{false};

  const auto path_end = path.path_to_value.rend();
  for (auto path_iter = path.path_to_value.rbegin(); path_iter != path_end; ++path_iter) {
    if (!llvm::isa<llvm::GEPOperator>(*path_iter)) {
      continue;
    }

    has_gep = true;

    auto* gep = llvm::cast<llvm::GEPOperator>(*path_iter);

    LOG_DEBUG("Path iter gep for extraction is currently " << *gep);
    type = gep::extract_gep_deref_type(type.value(), *gep);

    if (type) {
      LOG_DEBUG("Extracted type w.r.t. gep: " << log::ditype_str(*type));
      type = reset_ditype(type.value(), path_iter, path_end).value_or(type.value());
    }
  }

  if (type && !has_gep) {
    // handle load of pointer (without gep, see heap_matrix_simple.c)
    // FIXME: rbegin might be wrong here, need a better anchor, as the function only loos at next(rbegin()), which is
    // usually a gep, but not with this call.
    type = reset_ditype(type.value(), path.path_to_value.rbegin(), path_end).value_or(type.value());
  }

  if (type) {
    type = tbaa::resolve_tbaa(type.value(), path);
  }

  return type;
}

llvm::SmallVector<llvm::DIType*, 4> collect_types(llvm::ArrayRef<dataflow::ValuePath> paths_to_type) {
  using namespace llvm;
  SmallVector<llvm::DIType*, 4> di_types;
  llvm::transform(paths_to_type, dimeta::util::optional_back_inserter(di_types),
                  [](const auto& path) { return find_type(path); });
  return di_types;
}

}  // namespace ditype

std::optional<llvm::DIType*> type_for_malloclike(const llvm::CallBase* call) {
  const auto ditype_paths = dataflow::type_for_heap_call(call);

  const auto ditypes_vector = ditype::collect_types(ditype_paths);
  if (ditypes_vector.empty()) {
    return {};
  }
  return *ditypes_vector.begin();
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
    LOG_TRACE("Type for new-like " << cb_fun->getName())
    extracted_type = type_for_newlike(call);
    // !heapallocsite gives the type after "new", i.e., new int -> int, new int*[n] -> int*.
    // Our malloc-related algorithm would return int* and int** respectively, however, hence:
    pointer_level_offset += 1;
  }
#endif

  if (!extracted_type) {
    LOG_TRACE("Type for malloc-like: " << cb_fun->getName())
    extracted_type = type_for_malloclike(call);
  }
  const auto lang                        = is_cxx_new ? DimetaData::Lang::CXX : DimetaData::Lang::C;
  const auto [final_type, pointer_level] = final_ditype(extracted_type);
  const auto meta =
      DimetaData{lang, DimetaData::MemLoc::Heap, {}, extracted_type, final_type, pointer_level + pointer_level_offset};
  return meta;
}

std::optional<DimetaData> type_for(const llvm::AllocaInst* ai) {
  const auto local_di_var = ditype::find_local_var_for(ai);
  const auto lang         = DimetaData::Lang::C;

  if (local_di_var) {
    auto extracted_type                    = local_di_var.value()->getType();
    const auto [final_type, pointer_level] = final_ditype(extracted_type);
    const auto meta =
        DimetaData{lang, DimetaData::MemLoc::Stack, local_di_var, extracted_type, final_type, pointer_level};
    return meta;
  }

  return {};
}

std::optional<DimetaData> type_for(const llvm::GlobalVariable* gv) {
  llvm::SmallVector<llvm::DIGlobalVariableExpression*, 2> dbg_info;
  gv->getDebugInfo(dbg_info);
  if (!dbg_info.empty()) {
    const auto lang                        = DimetaData::Lang::C;
    auto gv_expr                           = *dbg_info.begin();
    auto gv_type                           = gv_expr->getVariable()->getType();
    const auto [final_type, pointer_level] = final_ditype(gv_type);
    return DimetaData{lang, DimetaData::MemLoc::Global, {}, gv_type, final_type, pointer_level};
  }
  return {};
}

}  // namespace dimeta
