//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef LIB_TYPE_DIPATH_H
#define LIB_TYPE_DIPATH_H

#include "support/Logger.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Value.h"

#include <optional>
#include <string>

namespace llvm {
class raw_ostream;
}

namespace dimeta::type::dipath {

struct IRMapping {
  const llvm::Value* value{nullptr};
  llvm::DIType* mapped{nullptr};
  std::string reason;
};

struct ValueToDiPath {
  llvm::SmallVector<IRMapping, 8> path_to_ditype;

  void emplace_back(const llvm::Value* val, llvm::DIType* mapped_di_type, const std::string reason = "");

  std::optional<llvm::DIType*> final_type() const;
};

llvm::raw_ostream& operator<<(llvm::raw_ostream& os, const ValueToDiPath& vdp);

}  // namespace dimeta::type::dipath

#endif  // LIB_TYPE_DIPATH_H