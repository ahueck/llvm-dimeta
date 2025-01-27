//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DITYPEEXTRACTOR_H
#define DIMETA_DITYPEEXTRACTOR_H

#include <llvm/IR/InstrTypes.h>
#include <optional>

namespace llvm {
class DIType;
}

namespace dimeta::dataflow {
struct CallValuePath;
}

namespace dimeta::type {

std::optional<llvm::DIType*> find_type(const dataflow::CallValuePath& call_path);

}

#endif
