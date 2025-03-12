//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIVISITORUTILS_H
#define DIMETA_DIVISITORUTILS_H

#include <llvm/IR/DebugInfoMetadata.h>
#include <optional>

namespace llvm {
class DINode;
class raw_ostream;
class Module;
class DIDerivedType;
class DIType;
class DICompositeType;
}  // namespace llvm

namespace dimeta::visitor::util {

void print_dinode(llvm::DINode*, llvm::raw_ostream&, llvm::Module* m = nullptr);

struct StructMember {
  std::optional<llvm::DIDerivedType*> member{};
  std::optional<llvm::DIType*> type_of_member;
};

std::optional<StructMember> resolve_byte_offset_to_member_of(llvm::DICompositeType* composite, unsigned byte_offset);

}  // namespace dimeta::visitor::util

#endif  // DIMETA_DIVISITORUTILS_H
