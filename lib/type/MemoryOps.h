//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_MEMORYOPS_H
#define DIMETA_MEMORYOPS_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

#include <cstdint>

namespace dimeta::memory {

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
  [[nodiscard]] inline std::optional<MemOpKind> allocKind(llvm::StringRef function) const {
    if (auto it = alloc_map_.find(function); it != std::end(alloc_map_)) {
      return {(*it).second};
    }
    return {};
  }

  [[nodiscard]] inline bool isNewLike(llvm::StringRef function) const {
    auto kind = allocKind(function);
    if (!kind) {
      return false;
    }
    const auto value = kind.value();
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

}  // namespace dimeta::memory

#endif  // DIMETA_MEMORYOPS_H
