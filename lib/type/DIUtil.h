//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIUTILS_H
#define DIMETA_DIUTILS_H

#include "llvm/ADT/SmallVector.h"

#include <optional>

namespace llvm {
class DINode;
class raw_ostream;
class Module;
class DIDerivedType;
class DIType;
class DICompositeType;
}  // namespace llvm

namespace dimeta::di::util {

void print_dinode(llvm::DINode*, llvm::raw_ostream&, llvm::Module* m = nullptr);

struct StructMember {
  std::optional<llvm::DIDerivedType*> member{};
  std::optional<llvm::DIType*> type_of_member;
};

std::optional<StructMember> resolve_byte_offset_to_member_of(const llvm::DICompositeType* composite,
                                                             size_t byte_offset);

bool is_pointer(const llvm::DIType& di_type);
bool is_pointer_like(const llvm::DIType& di_type);
bool is_non_static_member(const llvm::DINode& elem);
size_t get_num_composite_members(const llvm::DICompositeType& composite);
llvm::SmallVector<llvm::DIDerivedType*, 4> get_composite_members(const llvm::DICompositeType& composite);

}  // namespace dimeta::di::util

#endif  // DIMETA_DIVISITORUTILS_H
