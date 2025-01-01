//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DITYPEEXTRACTOR_H
#define DIMETA_DITYPEEXTRACTOR_H

#include <optional>

namespace dimeta::dataflow {
struct ValuePath;
}  // namespace dimeta::dataflow

namespace llvm {
class DIType;
}

namespace dimeta::type {

std::optional<llvm::DIType*> find_type(const dataflow::ValuePath& path);

}

#endif
