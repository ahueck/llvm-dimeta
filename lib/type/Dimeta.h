//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIMETA_H
#define DIMETA_DIMETA_H

#include <optional>

namespace llvm {
class AllocaInst;
class CallBase;
class DILocalVariable;
class DIType;
class Value;
}  // namespace llvm

namespace dimeta {

struct DimetaData {
  enum Lang { C = 0, CXX };
  Lang language{C};
  std::optional<llvm::DILocalVariable*> stack_alloca{};
  std::optional<llvm::DIType*> entry_type{};
  std::optional<llvm::DIType*> base_type{};
  int pointer_level{0};
};

std::optional<DimetaData> type_for(const llvm::AllocaInst*);

std::optional<DimetaData> type_for(const llvm::CallBase*);

}  // namespace dimeta

#endif  // DIMETA_DIMETA_H
