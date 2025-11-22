#include "DIFinder.h"
#include "DIRootType.h"
#include "DIUtil.h"
#include "DataflowAnalysis.h"
#include "DefUseAnalysis.h"
#include "DimetaData.h"
#include "GEP.h"
#include "TBAA.h"
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
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace dimeta {
namespace fortran {

struct TypeDescritor {
  // { ptr, i64, i32, i8, i8, i8, i8, [1 x [3 x i64]], ptr, [1 x i64] }
};

std::optional<llvm::DIType*> extract(const dataflow::CallValuePath& call_path, std::optional<llvm::DIType*> type) {
  assert(call_path.call.has_value() && "Expected a Fortran call handle");
  // Fortran workaround:
  // If _FortranAAllocatableAllocate called on a global directly, assume first member is actually allocated, see
  // test 08_bounds_nogep.f90 vs. 08_bounds.f90:
  // @_QMtea_moduleEchunk = global %_QMtea_moduleTchunktype
  // call i32 @_FortranAAllocatableAllocate(ptr @_QMtea_moduleEchunk, ...), !dbg !30

  LOG_DEBUG("Fortran workaround for _FortranAAllocatableAllocate on a global variable")

  const bool only_global = [&]() -> bool {
    bool is_global_target = llvm::isa<llvm::GlobalVariable>(*call_path.path.value()) && call_path.path.size() == 1;

    // 09_local_bounds.f90 optimized: (alloca -> memcpy of global) [->] allocatable:
    for (auto user : call_path.path.value().value()->users()) {
      if (llvm::MemCpyInst::classof(user)) {
        is_global_target = llvm::isa<llvm::GlobalVariable>(llvm::cast<llvm::MemCpyInst>(user)->getSource());
      }
    }
    return is_global_target;
  }();

  auto ditype_final = type.value();

  if (only_global && llvm::isa<llvm::DICompositeType>(ditype_final)) {
    // dipath.emplace_back(call_path.path.value(), type.value()->get)
    LOG_FATAL("Reset fortran allocated type " << log::ditype_str(ditype_final))
    auto struct_mem = di::util::resolve_byte_offset_to_member_of(llvm::cast<llvm::DICompositeType>(ditype_final), 0);
    if (struct_mem) {
      // dipath.emplace_back(*call_path.path.value(), struct_mem->type_of_member.value_or(ditype_final),
      // "Fortran global reset");
      return struct_mem->type_of_member.value_or(ditype_final);
    }
  }

  return ditype_final;
}
}  // namespace fortran
}  // namespace dimeta