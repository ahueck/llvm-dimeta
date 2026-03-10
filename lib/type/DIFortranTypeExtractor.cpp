#include "DIFinder.h"
#include "DIPath.h"
#include "DIRootType.h"
#include "DIUtil.h"
#include "DataflowAnalysis.h"
#include "GEP.h"
#include "TBAA.h"
#include "ValuePath.h"
#include "support/Logger.h"

#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <llvm/IR/DerivedTypes.h>
#include <optional>

namespace dimeta::fortran {

struct TypeDescritor {
  // { ptr, i64, i32, i8, i8, i8, i8, [1 x [3 x i64]], ptr, [1 x i64] }
};

bool is_fortran_descriptor(llvm::Type* type) {
  LOG_DEBUG("Analyzing " << *type)
  // Detects if the given type is a Fortran descriptor structure.
  // Expected pattern:
  // - Array: { ptr, i64, i32, i8, i8, i8, i8, [n x [3 x i64]], ptr, [n x i64] } (10 elements)
  // - Scalar: { ptr, i64, i32, i8, i8, i8, i8, ptr, [n x i64] } (9 elements)
  auto* struct_type = llvm::dyn_cast_or_null<llvm::StructType>(type);
  if (!struct_type) {
    return false;
  }
  const auto num_elements = struct_type->getNumElements();
  if (num_elements != 10 && num_elements != 9) {
    return false;
  }
  const bool is_scalar = (num_elements == 9);
  // 0: ptr (base_addr)
  if (!struct_type->getElementType(0)->isPointerTy()) {
    return false;
  }
  // 1: i64 (elem_len)
  if (!struct_type->getElementType(1)->isIntegerTy(64)) {
    return false;
  }
  // 2: i32 (version)
  if (!struct_type->getElementType(2)->isIntegerTy(32)) {
    return false;
  }
  // 3-6: i8 (rank, type, attribute, f18Addendum)
  for (unsigned i = 3; i <= 6; ++i) {
    if (!struct_type->getElementType(i)->isIntegerTy(8)) {
      return false;
    }
  }
  unsigned current_idx     = 7;
  llvm::ArrayType* dim_arr = nullptr;
  if (!is_scalar) {
    // 7: [n x [3 x i64]] (dimensions)
    dim_arr = llvm::dyn_cast<llvm::ArrayType>(struct_type->getElementType(current_idx++));
    if (!dim_arr) {
      return false;
    }
    auto* inner_dim_arr = llvm::dyn_cast<llvm::ArrayType>(dim_arr->getElementType());
    if (!inner_dim_arr || inner_dim_arr->getNumElements() != 3 || !inner_dim_arr->getElementType()->isIntegerTy(64)) {
      return false;
    }
  }
  // 8 (or 7): ptr (type descriptor pointer)
  if (!struct_type->getElementType(current_idx++)->isPointerTy()) {
    return false;
  }
  // 9 (or 8): [n x i64] (addendum)
  auto* addendum_arr = llvm::dyn_cast<llvm::ArrayType>(struct_type->getElementType(current_idx++));
  if (!addendum_arr || !addendum_arr->getElementType()->isIntegerTy(64)) {
    return false;
  }
  if (dim_arr && dim_arr->getNumElements() != addendum_arr->getNumElements()) {
    return false;
  }
  return true;
}

template <typename Iter>
std::optional<llvm::DIType*> reset_ditype(llvm::DIType* type_to_reset, const dataflow::ValuePath& path,
                                          const Iter& path_iter, type::dipath::ValueToDiPath& logged_dipath) {
  std::optional<llvm::DIType*> type = type_to_reset;

  const auto& current_value = path_iter;
  LOG_DEBUG("Type to reset: " << log::ditype_str(*type));
  LOG_DEBUG(">> based on IR: " << **current_value);

  if (llvm::isa<llvm::GEPOperator>(*current_value)) {
    LOG_DEBUG("Reset based on GEP")
    auto* gep                     = llvm::cast<llvm::GEPOperator>(*current_value);
    const bool fortran_descriptor = fortran::is_fortran_descriptor(gep->getSourceElementType());
    if (!fortran_descriptor) {
      const auto gep_result = gep::extract_gep_dereferenced_type(type.value(), *gep);
      if (gep_result.member && !gep_result.use_type) {
        LOG_DEBUG("Using gep member type result")
        type = gep_result.member;
      } else {
        type = gep_result.type;
      }
    }
  } else if (const auto* load = llvm::dyn_cast<llvm::LoadInst>(*current_value)) {
    LOG_DEBUG("Reset based on load " << *load)
    // TODO
  } else if (const auto* store_inst = llvm::dyn_cast<llvm::StoreInst>(*current_value)) {
    LOG_DEBUG("Reset based on store " << *store_inst)

  } else {
    LOG_DEBUG(">> skipping: " << **current_value);
  }

  logged_dipath.emplace_back(*current_value, type.value_or(nullptr));

  return type;
}

std::optional<llvm::DIType*> extract(const dataflow::CallValuePath& call_path, std::optional<llvm::DIType*> type) {
  assert(call_path.call.has_value() && "Expected a Fortran call handle");
  // Fortran workaround:
  // If _FortranAAllocatableAllocate called on a global directly, assume first member is actually allocated, see
  // test 08_bounds_nogep.f90 vs. 08_bounds.f90:
  // @_QMtea_moduleEchunk = global %_QMtea_moduleTchunktype
  // call i32 @_FortranAAllocatableAllocate(ptr @_QMtea_moduleEchunk, ...), !dbg !30

  LOG_DEBUG("Fortran workaround for _FortranAAllocatableAllocate")

  type::dipath::ValueToDiPath dipath;

  const auto path_end = call_path.path.path_to_value.rend();
  for (auto path_iter = call_path.path.path_to_value.rbegin(); path_iter != path_end; ++path_iter) {
    LOG_DEBUG("Extracted type: " << log::ditype_str(*type));
    type = reset_ditype(type.value(), call_path.path, path_iter, dipath).value_or(type.value());
    LOG_DEBUG("reset_ditype result " << log::ditype_str(type.value_or(nullptr)) << "\n")
    if (!type) {
      break;
    }
  }

  auto* ditype_final = type.value();

  const bool only_global = [&]() -> bool {
    bool is_global_target = llvm::isa<llvm::GlobalVariable>(*call_path.path.value()) && call_path.path.size() == 1;

    // 09_local_bounds.f90 optimized: (alloca -> memcpy of global) [->] allocatable:
    for (const auto* user : call_path.path.value().value()->users()) {
      if (llvm::MemCpyInst::classof(user)) {
        is_global_target = llvm::isa<llvm::GlobalVariable>(llvm::cast<llvm::MemCpyInst>(user)->getSource());
      }
    }
    return is_global_target;
  }();

  if (only_global && llvm::isa<llvm::DICompositeType>(ditype_final)) {
    LOG_DEBUG("Reset fortran allocated type " << log::ditype_str(ditype_final))

    auto struct_mem = di::util::resolve_byte_offset_to_member_of(llvm::cast<llvm::DICompositeType>(ditype_final), 0);
    if (struct_mem) {
      dipath.emplace_back(nullptr, struct_mem->type_of_member.value_or(ditype_final), "Allocatable of global type");
    }
  }

  const auto global_inheritance = [&]() -> std::optional<llvm::GlobalVariable*> {
    const auto* global_target = llvm::dyn_cast<llvm::GlobalVariable>(*call_path.path.value());
    if (!global_target) {
      return {};
    }
    for (const auto* user : global_target->users()) {
      if (auto* call = llvm::dyn_cast<llvm::CallBase>(user)) {
        auto target    = call->getCalledFunction();
        auto is_target = target ? target->getName() == ("_FortranAAllocatableInitDerivedForAllocate") : false;
        if (is_target) {
          auto global = llvm::dyn_cast<llvm::GlobalVariable>(call->getOperand(1));
          if (global) {
            return {global};
          }
          break;
        }
      }
    }
    return {};
  };
  auto type_inheritance = global_inheritance();
  if (type_inheritance) {
    LOG_DEBUG("Found inheritance, type descriptor: " << **type_inheritance)
  }

  LOG_DEBUG("Final mapping\n" << dipath)

  return dipath.final_type();
}
}  // namespace dimeta::fortran
