//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "Dimeta.h"

#include "DIVisitor.h"
#include "DefUseAnalysis.h"
#include "MetaIO.h"
#include "MetaParse.h"

#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Operator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
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

namespace memory {

enum class MemOpKind : uint8_t {
  kNewLike          = 1 << 0,  // allocates, never null
  kMallocLike       = 1 << 1,  // allocates, maybe null
  kAlignedAllocLike = 1 << 2,  // allocates aligned, maybe null
  kCallocLike       = 1 << 3,  // allocates zeroed
  kReallocLike      = 1 << 4,  // re-allocated (existing) memory
  kNewCppLike       = ((1 << 5) | kNewLike)
};

namespace detail {
template <class T>
constexpr bool has_value(T flags, T value) {
  return (std::underlying_type_t<T>)flags & (std::underlying_type_t<T>)value;
}
}  // namespace detail

struct MemOps {
  [[nodiscard]] inline llvm::Optional<MemOpKind> allocKind(llvm::StringRef function) const {
    if (auto it = alloc_map_.find(function); it != std::end(alloc_map_)) {
      return {(*it).second};
    }
    return llvm::None;
  }

  [[nodiscard]] inline bool isNewLike(llvm::StringRef function) const {
    auto kind = allocKind(function);
    if (!kind) {
      return false;
    }
    const auto value = kind.getValue();
    return detail::has_value(value, MemOpKind::kNewCppLike);
  }

  [[nodiscard]] inline bool isAlloc(llvm::StringRef function) const {
    auto kind = allocKind(function);
    return static_cast<bool>(kind);
  }

 private:
  const llvm::StringMap<MemOpKind> alloc_map_{
      {"malloc", MemOpKind::kMallocLike},
      {"calloc", MemOpKind::kCallocLike},
      {"realloc", MemOpKind::kReallocLike},
      {"aligned_alloc", MemOpKind::kAlignedAllocLike},
      {"_Znwm", MemOpKind::kNewLike},                                 /*new(unsigned long)*/
      {"_Znwj", MemOpKind::kNewLike},                                 /*new(unsigned int)*/
      {"_Znam", MemOpKind::kNewLike},                                 /*new[](unsigned long)*/
      {"_Znaj", MemOpKind::kNewLike},                                 /*new[](unsigned int)*/
      {"_ZnwjRKSt9nothrow_t", MemOpKind::kNewCppLike},                /*new(unsigned int, nothrow)*/
      {"_ZnwmRKSt9nothrow_t", MemOpKind::kNewCppLike},                /*new(unsigned long, nothrow)*/
      {"_ZnajRKSt9nothrow_t", MemOpKind::kNewCppLike},                /*new[](unsigned int, nothrow)*/
      {"_ZnamRKSt9nothrow_t", MemOpKind::kNewCppLike},                /*new[](unsigned long, nothrow)*/
      {"_ZnwjSt11align_val_t", MemOpKind::kNewLike},                  /*new(unsigned int, align_val_t)*/
      {"_ZnwjSt11align_val_tRKSt9nothrow_t", MemOpKind::kNewCppLike}, /*new(unsigned int, align_val_t, nothrow)*/
      {"_ZnwmSt11align_val_t", MemOpKind::kNewCppLike},               /*new(unsigned long, align_val_t)*/
      {"_ZnwmSt11align_val_tRKSt9nothrow_t", MemOpKind::kNewCppLike}, /*new(unsigned long, align_val_t, nothrow)*/
      {"_ZnajSt11align_val_t", MemOpKind::kNewLike},                  /*new[](unsigned int, align_val_t)*/
      {"_ZnajSt11align_val_tRKSt9nothrow_t", MemOpKind::kNewCppLike}, /*new[](unsigned int, align_val_t, nothrow)*/
      {"_ZnamSt11align_val_t", MemOpKind::kNewLike},                  /*new[](unsigned long, align_val_t)*/
      {"_ZnamSt11align_val_tRKSt9nothrow_t", MemOpKind::kNewCppLike}, /*new[](unsigned long, align_val_t, nothrow)*/
  };
};

}  // namespace memory

namespace dataflow {

struct MallocAnchorMatcher {
  const llvm::CallBase* malloc_like_call{nullptr};
  llvm::SmallVector<ValuePath, 4> anchors;

  explicit MallocAnchorMatcher(const llvm::CallBase* call) : malloc_like_call{call} {
  }

  [[nodiscard]] const llvm::Module* getModule() const {
    return malloc_like_call->getParent()->getParent()->getParent();
  }

  [[nodiscard]] const llvm::Function* getFunction() const {
    return malloc_like_call->getParent()->getParent();
  }

  auto operator()(const ValuePath& path) -> decltype(DefUseChain::kContinue) {
    using namespace llvm;
    using dataflow::DefUseChain;
    using dataflow::ValuePath;
    const auto* inst = dyn_cast<Instruction>(path.value());
    if (!inst) {
      return DefUseChain::kSkip;
    }

    if (isa<IntrinsicInst>(inst)) {
      return DefUseChain::kSkip;
    }

    switch (inst->getOpcode()) {
      case Instruction::Ret: {
        outs() << "Matched Ret! " << *inst << "\n";
        outs() << "  Path: " << path << "\n";
        anchors.push_back(path);
        return DefUseChain::kCancel;
      }
      case Instruction::Store: {
        outs() << "Matched store! " << *inst << "\n";
        outs() << "  Path: " << path << "\n";
        anchors.push_back(path);
        return DefUseChain::kCancel;
      }
      case Instruction::Call:  // NOLINT
        [[fallthrough]];
      case Instruction::CallBr:
        [[fallthrough]];
      case Instruction::Invoke: {
        if (path.only_start()) {
          return DefUseChain::kContinue;
        }
        outs() << "Matched call! " << *inst << "\n";
        outs() << "  Path: " << path << "\n";

        anchors.push_back(path);

        //        const auto* call_inst = llvm::cast<CallBase>(inst);
        //        const auto* called_f  = call_inst->getCalledFunction();
        //        if (called_f != nullptr) {
        //          outs() << "Matched callbase! " << *called_f << "\n";
        //          // Argument passed to current call:
        //          const auto* arg_val = path.previous_value();
        //          assert(arg_val != nullptr && "Previous value should be argument to some function!");
        //
        //          // Argument number:
        //          const auto* const arg_pos = llvm::find_if(
        //              call_inst->args(), [&arg_val](const auto& arg_use) -> bool { return arg_use.get() == arg_val;
        //              });
        //          if (arg_pos == call_inst->arg_end()) {
        //            return DefUseChain::kContinue;
        //          }
        //          auto arg_num = std::distance(call_inst->arg_begin(), arg_pos);
        //          // Extract debug info from function at arg_num:
        //          if (auto* sub_program = called_f->getSubprogram(); sub_program != nullptr) {
        //            const auto sub_prog_arg_pos = arg_num + 1;
        //            auto types_of_subprog       = sub_program->getType()->getTypeArray();
        //            assert((types_of_subprog.size() > sub_prog_arg_pos) && "Type array smaller than arg num!");
        //            auto* type = types_of_subprog[sub_prog_arg_pos];
        //            util::DIPrinter printer(llvm::outs(), getModule());
        //            printer.traverseType(type);
        //            return DefUseChain::kCancel;
        //          }
        //        }
        return DefUseChain::kContinue;
      }
    }
    return DefUseChain::kContinue;
  }
};

struct MallocTargetMatcher {
  llvm::SmallVector<dataflow::ValuePath, 4> types_path;

  auto operator()(const dataflow::ValuePath& path) -> decltype(dataflow::DefUseChain::kContinue) {
    using namespace llvm;
    using dataflow::DefUseChain;
    using dataflow::ValuePath;

    const auto* const value = path.value();
    // Handle path to alloca -> can extract type
    if (const auto* inst = dyn_cast<Instruction>(value)) {
      if (isa<IntrinsicInst>(inst)) {
        return DefUseChain::kSkip;
      }
      if (isa<AllocaInst>(inst)) {
        types_path.emplace_back(path);
        return DefUseChain::kSkip;  // maybe kCancel is better?
      }
    }

    if (isa<llvm::Argument>(value)) {
      types_path.emplace_back(path);
      return DefUseChain::kSkip;
    }

    if (llvm::isa<llvm::GlobalVariable>(value)) {
      types_path.emplace_back(path);
      return DefUseChain::kSkip;
    }

    if (const auto* const_expr = llvm::dyn_cast<llvm::ConstantExpr>(value)) {
      dbgs() << "Unsupported: Found constant expr: " << *const_expr << "\n";
    }

    return DefUseChain::kContinue;
  }
};

struct MallocBacktrackSearch {
  using ValueRange = llvm::TinyPtrVector<const llvm::Value*>;

  auto operator()(const dataflow::ValuePath& path) -> llvm::Optional<ValueRange> {
    using namespace llvm;
    using dataflow::DefUseChain;
    using dataflow::ValuePath;

    ValueRange result;

    if (auto const_expr = llvm::dyn_cast<llvm::ConstantExpr>(path.value())) {
      if (const_expr->getOpcode() == Instruction::GetElementPtr) {
        if (auto op = llvm::dyn_cast<llvm::GEPOperator>(const_expr)) {
          //          dbgs() << "Gep Constant casted. ";
          //          dbgs() << "gep of " << *op->getPointerOperand() << "\n";
          result.push_back(op->getPointerOperand());
        }
        return result;
      }
    }

    const auto* inst = dyn_cast<Instruction>(path.value());
    if (inst == nullptr) {
      return llvm::None;
    }

    switch (inst->getOpcode()) {
      case Instruction::Store: {
        //        if (path.start_value() == inst) {
        //          return llvm::None;
        //        }
        result.push_back(llvm::dyn_cast<StoreInst>(inst)->getPointerOperand());
        return result;
      }
      case Instruction::Load: {
        result.push_back(llvm::dyn_cast<LoadInst>(inst)->getPointerOperand());
        return result;
      }
      case Instruction::GetElementPtr: {
        result.push_back(llvm::dyn_cast<GetElementPtrInst>(inst)->getPointerOperand());
        return result;
      }
      case Instruction::BitCast: {
        result.push_back(llvm::dyn_cast<llvm::BitCastInst>(inst)->getOperand(0));
        return result;
      }
    }
    return llvm::None;
  }
};

}  // namespace dataflow

llvm::Optional<llvm::DIType*> type_for_newlike(const llvm::CallBase* call) {
  auto* heapalloc_md = call->getMetadata("heapallocsite");
  assert(heapalloc_md != nullptr && "Missing required heapallocsite metadata.");
  if (auto* type = llvm::dyn_cast<llvm::DIType>(heapalloc_md)) {
    //    util::DIPrinter printer(llvm::outs(), call->getParent()->getParent()->getParent());
    //    printer.traverseType(type);
    llvm::dbgs() << "Final Type: " << *type << "\n";
    return type;
  }
  return llvm::None;
}

namespace ditype {

namespace util {
bool contains_gep(const dataflow::ValuePath& path) {
  return llvm::find_if(path.path_to_value, [](const auto* val) { return llvm::isa<llvm::GetElementPtrInst>(val); }) !=
         std::end(path.path_to_value);
}
}  // namespace util

llvm::Optional<llvm::DIType*> find_type_root(const dataflow::ValuePath& path) {
  using namespace llvm;
  const auto* value = path.value();

  if (const auto* ret = dyn_cast<ReturnInst>(value)) {
    auto* sub_prog  = ret->getFunction()->getSubprogram();
    auto type_array = sub_prog->getType()->getTypeArray();
    if (type_array.size() > 0) {
      return {*type_array.begin()};
    }
    return None;
  }

  if (const auto* alloca = dyn_cast<AllocaInst>(value)) {
    auto local_di_var = type_for(alloca);
    if (local_di_var) {
      return local_di_var.getValue()->getType();
    }
    return None;
  }

  if (const auto* call_inst = llvm::dyn_cast<CallBase>(value)) {
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
    return None;
  }

  if (const auto* global_variable = llvm::dyn_cast<llvm::GlobalVariable>(value)) {
    auto dbg_md = global_variable->getMetadata("dbg");
    if (!dbg_md) {
      return None;
    }
    if (auto* global_expression = llvm::dyn_cast<llvm::DIGlobalVariableExpression>(dbg_md)) {
      return global_expression->getVariable()->getType();
    }
    return None;
  }

  if (const auto* argument = llvm::dyn_cast<llvm::Argument>(value)) {
    if (auto* subprogram = argument->getParent()->getSubprogram(); subprogram != nullptr) {
      const auto arg_pos    = argument->getArgNo() + 1;
      const auto type_array = subprogram->getType()->getTypeArray();
      return type_array[arg_pos];
    }

    return None;
  }

  if (const auto* const_expr = llvm::dyn_cast<llvm::ConstantExpr>(value)) {
    // TODO
    llvm::dbgs() << "find_type: ConstantExpr unsupported\n";
  }

  llvm::dbgs() << "find_type: No matching value found for " << *value << "\n";
  return None;
}

struct GepIndices {
  const llvm::GetElementPtrInst* gep_inst;
  llvm::SmallVector<uint64_t, 4> indices_;
  using Iter = llvm::SmallVector<uint64_t, 4>::const_iterator;

  inline llvm::iterator_range<Iter> indices() const {
    return llvm::iterator_range<Iter>(indices_);
  }

  inline size_t size() const {
    return indices_.size();
  }

  static GepIndices create(const llvm::GetElementPtrInst* inst, bool skip_first = true) {
    GepIndices gep_ind;
    gep_ind.gep_inst = inst;

    for (const auto& index : inst->indices()) {
      if (skip_first) {
        skip_first = false;
        continue;
      }
      if (auto const_idx = llvm::dyn_cast<llvm::ConstantInt>(index.get())) {
        int64_t index = const_idx->getValue().getSExtValue();
        gep_ind.indices_.emplace_back(index);
      }
    }
    return gep_ind;
  }
  static GepIndices create(const llvm::GEPOperator* inst, bool skip_first = true) {
    GepIndices gep_ind;
    //    gep_ind.gep_inst = inst;

    for (const auto& index : inst->indices()) {
      if (skip_first) {
        skip_first = false;
        continue;
      }
      if (auto const_idx = llvm::dyn_cast<llvm::ConstantInt>(index.get())) {
        int64_t index = const_idx->getValue().getSExtValue();
        gep_ind.indices_.emplace_back(index);
      }
    }
    return gep_ind;
  }
};

inline llvm::raw_ostream& operator<<(llvm::raw_ostream& os, const GepIndices& indices) {
  const auto& vec = indices.indices_;
  if (vec.empty()) {
    os << "[]";
    return os;
  }
  const auto* begin = std::begin(vec);
  os << "[" << *begin;
  std::for_each(std::next(begin), std::end(vec), [&](const auto value) {
    os << ", ";
    os << value;
  });
  os << "]";
  return os;
}

llvm::Optional<llvm::DIType*> gep2ditype(llvm::DICompositeType* composite_type, const GepIndices& inst) {
  //  inst.idx_begin()

  const auto has_next_idx = [&inst](size_t pos) { return pos + 1 < inst.size(); };

  llvm::dbgs() << "Gep: " << inst << "\n";
  int index_counter{-1};
  for (const auto& index : inst.indices()) {
    ++index_counter;

    const auto& elems = composite_type->getElements();
    assert(elems.size() > index);

    auto element = elems[index];

    llvm::dbgs() << " element: " << *element << "\n";

    if (auto derived_type = llvm::dyn_cast<llvm::DIDerivedType>(element)) {
      assert(derived_type->getTag() == llvm::dwarf::DW_TAG_member);
      auto member_type = derived_type->getBaseType();

      if (auto composite_member_type = llvm::dyn_cast<llvm::DICompositeType>(member_type)) {
        if (composite_member_type->getTag() == llvm::dwarf::DW_TAG_class_type ||
            composite_member_type->getTag() == llvm::dwarf::DW_TAG_structure_type) {
          // maybe need to recurse into!
          if (has_next_idx(index_counter)) {
            llvm::dbgs() << "Has next\n";
            //            llvm::dbgs() << "C " << index_counter << " s " << inst.size() << "\n";
            composite_type = composite_member_type;
            continue;
          }
        }
        if (composite_member_type->getTag() == llvm::dwarf::DW_TAG_array_type) {
          //          llvm::dbgs() << ">>C " << index_counter << " s " << inst.size() << "\n";
          // || composite_type->getTag() == llvm::dwarf::DW_TAG_array_type
          if (has_next_idx(index_counter)) {
            // At end of gep instruction, return basetype:
            //            llvm::dbgs() << " return base-type\n";
            return composite_member_type->getBaseType();
          }
          // maybe need to recurse into tag_array_type (of non-basic type...)
        }
      }

      return member_type;
    }
  }
  return llvm::None;
}

llvm::Optional<llvm::DIType*> extract_gep_type(llvm::DIType* root, const llvm::GEPOperator& inst) {
  using namespace llvm;

  auto gep_src = inst.getSourceElementType();
  if (gep_src->isPointerTy()) {
    llvm::dbgs() << "Gep to ptr\n";
    return root;  // basetype
  }

  if (gep_src->isArrayTy()) {
    llvm::dbgs() << "Gep to array\n";
    if (auto composite_type = llvm::dyn_cast<llvm::DICompositeType>(root)) {
      return composite_type->getBaseType();
    }
    return root;
  }

  const auto find_composite = [](llvm::DIType* root) {
    llvm::DIType* type = root;
    while (llvm::isa<llvm::DIDerivedType>(type)) {
      auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
      type        = ditype->getBaseType();
    }
    return type;
  };

  auto composite_type = llvm::dyn_cast<llvm::DICompositeType>(find_composite(root));
  assert(composite_type != nullptr && "Root should be a struct-like type.");

  auto accessed_ditype = gep2ditype(composite_type, GepIndices::create(&inst));

  return accessed_ditype;
}

llvm::Optional<llvm::DIType*> find_type(const dataflow::ValuePath& path) {
  auto type = find_type_root(path);

  //  for (auto* gep : llvm::make_filter_range(llvm::make_range(path.path_to_value.rbegin(), path.path_to_value.rend()),
  //                                           [](auto* value) { return llvm::isa<llvm::GEPOperator>(value); })) {
  const auto gep_count =
      llvm::count_if(path.path_to_value, [](auto* val) { return llvm::isa<llvm::GEPOperator>(val); });
  int current_gep_count{0};
  const auto path_end = path.path_to_value.rend();
  for (auto path_iter = path.path_to_value.rbegin(); path_iter != path_end; ++path_iter) {
    if (!llvm::isa<llvm::GEPOperator>(*path_iter)) {
      continue;
    }
    ++current_gep_count;
    auto* gep = *path_iter;
    llvm::dbgs() << "Iter " << *gep << "\n";
    type = extract_gep_type(type.getValue(), *llvm::cast<llvm::GEPOperator>(gep));
    if (type.hasValue()) {
      //      type = extract_gep_type(type.getValue(), *llvm::cast<llvm::GEPOperator>(gep));
      if (type.hasValue()) {
        llvm::dbgs() << "Extracted type: " << **type << "\n";

        // Look at next value after gep (e.g., a load or the final store)
        auto next_value = std::next(path_iter);
        if (next_value == path_end) {
          continue;
        }

        // Reset the DIType from the gep:
        // - a store with a ditype(array) is likely the first element of the array
        // - a load ofter a gep is likely the first element of the composite type
        llvm::dbgs() << "Looking at " << **next_value << "\n";
        if (const auto* value = llvm::dyn_cast<llvm::LoadInst>(*next_value)) {
          // workaround for gep/array_composite_sub.c (non-optim/optim):
          auto next_after_load = std::next(next_value);
          assert(next_after_load != path_end && "After load there should be a instruction!");
          if (!llvm::isa<llvm::StoreInst>(*next_after_load)) {
            // a load resolves a pointer level in the DI type, but we only look at the final gep before the store.
            continue;
          }

          auto ditype_val = type.value();
          llvm::dbgs() << "With ditype " << *ditype_val << "\n";
          if (auto* ptr_to_composite = llvm::dyn_cast<llvm::DIDerivedType>(ditype_val)) {
            auto base_type = ptr_to_composite->getBaseType();
            assert(base_type != nullptr && "Pointer points to null-type (void*?)");

            if (auto* composite = llvm::dyn_cast<llvm::DICompositeType>(ptr_to_composite->getBaseType())) {
              //              if (composite->getTag() == llvm::dwarf::DW_TAG_array_type) {
              //                continue;
              //              }
              assert(!composite->getElements().empty() && "Load should target member of composite type!");

              auto first_elem = composite->getElements()[0];
              if (auto loaded_elem = llvm::dyn_cast<llvm::DIType>(first_elem)) {
                llvm::dbgs() << "Loaded from extracted type: " << *loaded_elem << "\n";
                type = loaded_elem;
              }
            }
          }
        }
        if (const auto* value = llvm::dyn_cast<llvm::StoreInst>(*next_value)) {
          auto ditype_val = type.value();
          llvm::dbgs() << "With stored ditype " << *ditype_val << "\n";
          if (auto* array_to_composite = llvm::dyn_cast<llvm::DICompositeType>(ditype_val)) {
            if (array_to_composite->getTag() == llvm::dwarf::DW_TAG_array_type) {
              type = array_to_composite->getBaseType();
            }
          }
        }

      } else {
        llvm::dbgs() << "Extracted type: None\n";
      }
    }
  }

  //  for (auto enum_gep : llvm::enumerate(llvm::make_range(path.path_to_value.rbegin(), path.path_to_value.rend())))
  //  {
  //    if (!llvm::isa<llvm::GEPOperator>(enum_gep.value())) {
  //      continue;
  //    }
  //    auto gep = enum_gep.value();
  //    if (type.hasValue()) {
  //      type = extract_gep_type(type.getValue(), *llvm::cast<llvm::GEPOperator>(gep));
  //      if (type.hasValue()) {
  //        llvm::dbgs() << "Extracted type: " << **type << " at " << enum_gep.index() << "\n";
  //
  //        //        llvm::dbgs() << (size_path - enum_gep.index() - 1) << "\n";
  //        auto next_value = path.at(enum_gep.index() + 1);
  //        llvm::dbgs() << **next_value << "<<\n";
  //        if (!next_value) {
  //          continue;
  //        }
  //
  //        if (const auto* value = llvm::dyn_cast<llvm::LoadInst>(next_value.value())) {
  //          auto ditype_val = type.value();
  //          if (auto* composite = llvm::dyn_cast<llvm::DICompositeType>(ditype_val)) {
  //            assert(!composite->getElements().empty() && "Load should target member of composite type!");
  //            auto first_elem = composite->getElements()[0];
  //            if (auto loaded_elem = llvm::dyn_cast<llvm::DIType>(first_elem)) {
  //              llvm::dbgs() << "Loaded from extracted type: " << *loaded_elem << "\n";
  //              type = loaded_elem;
  //            }
  //          }
  //        }
  //      } else {
  //        llvm::dbgs() << "Extracted type: None\n";
  //      }
  //    }
  //  }
  if (type.hasValue()) {
#if LLVM_VERSION_MAJOR > 12
    llvm::dbgs() << "  Type: " << *(type.value()) << "\n";
#else
    llvm::dbgs() << "  Type: " << *(type.getValue()) << "\n";
#endif
  } else {
    llvm::dbgs() << "  Type: empty\n";
  }

  //  if(path has load pointer after gep){  return "first" struct member }

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

llvm::Optional<llvm::DIType*> type_for_malloclike(const llvm::CallBase* call) {
  using namespace llvm;
  using dataflow::DefUseChain;
  using dataflow::ValuePath;
  using namespace dataflow;

  auto should_search = [&](const ValuePath&) -> bool { return true; };
  DefUseChain value_traversal;

  // forward: find pathes to anchor (store, ret, func call etc.) from malloc-like
  MallocAnchorMatcher malloc_anchor_finder{call};
  value_traversal.traverse(call, malloc_anchor_finder, should_search);

  // backward: find pathes from anchor (store) to alloca/argument/global etc.
  MallocTargetMatcher malloc_anchor_backtrack;
  MallocBacktrackSearch backtracker_search;

  assert(!malloc_anchor_finder.anchors.empty() && "Anchor should not be empty");
  llvm::SmallVector<dataflow::ValuePath, 4> ditype_paths;

  for (const auto& anchor_path : malloc_anchor_finder.anchors) {
    if (isa<StoreInst>(anchor_path.value())) {
      //      dbgs() << "Traverse " << anchor_path.value() << "\n";
      value_traversal.traverse_custom(anchor_path.value(), malloc_anchor_backtrack, should_search, backtracker_search);
      for (const auto& backtrack_path : malloc_anchor_backtrack.types_path) {
        ditype_paths.emplace_back(backtrack_path);
      }
      continue;
    }
    ditype_paths.emplace_back(anchor_path);
  }

  dbgs() << "Paths to types: \n";
  for (const auto& path : ditype_paths) {
    dbgs() << "  T: " << path << "\n";
  }

  const auto ditype_tostring = [](auto* ditype) {
    llvm::DIType* type = ditype;
    while (llvm::isa<llvm::DIDerivedType>(type)) {
      auto ditype = llvm::dyn_cast<llvm::DIDerivedType>(type);
      // void*-based derived types:
      if (ditype->getBaseType() == nullptr) {
        return type;
      }
      type = ditype->getBaseType();
    }

    return type;
  };

  const auto ditypes_vector = ditype::collect_types(ditype_paths);
  for (auto* ditype : ditypes_vector) {
    if (ditype == nullptr) {
      continue;
    }
    dbgs() << "Final Type: " << *ditype_tostring(ditype) << "\n\n";
  }
  if (ditypes_vector.empty()) {
    return None;
  }
  return *ditypes_vector.begin();
}

llvm::Optional<llvm::DIType*> type_for(const llvm::CallBase* call) {
  using namespace llvm;
  const dimeta::memory::MemOps mem_ops;

  auto* cb_fun = call->getCalledFunction();
  if (!cb_fun) {
    return None;
  }

  if (!mem_ops.isAlloc(cb_fun->getName())) {
    dbgs() << "Skipping call base: " << cb_fun->getName() << "\n";
    return None;
  }

  if (mem_ops.isNewLike(cb_fun->getName())) {
    return type_for_newlike(call);
  }

  dbgs() << "Type for malloc-like: " << cb_fun->getName() << "\n";
  return type_for_malloclike(call);
}

llvm::Optional<llvm::DILocalVariable*> type_for(const llvm::AllocaInst* ai) {
  using namespace llvm;
  //  DebugInfoFinder di_finder;
  const auto find_di_var = [&](auto* ai) -> llvm::Optional<DILocalVariable*> {
    auto& func = *ai->getFunction();
    for (auto& inst : llvm::instructions(func)) {
      if (auto* dbg = dyn_cast<DbgVariableIntrinsic>(&inst)) {
        if (compat::get_alloca_for(dbg) == ai) {
          //          di_finder.processInstruction(*ai->getModule(), inst);
          return dbg->getVariable();
        }
      }
    }
    return llvm::None;
  };

  return find_di_var(ai);
}

}  // namespace dimeta
