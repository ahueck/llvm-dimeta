//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIROOTTYPE_H
#define DIMETA_DIROOTTYPE_H

#include <optional>

namespace dimeta::dataflow {
struct CallValuePath;
}  // namespace dimeta::dataflow

namespace llvm {
class DIType;
class CallBase;
}  // namespace llvm

namespace dimeta::root {

std::optional<llvm::DIType*> find_type_root(const dataflow::CallValuePath& path);

}

#endif
