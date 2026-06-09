//  llvm-dimeta library
//  Copyright (c) 2022-2026 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef LIB_TYPE_DIFORTRANTYPEEXTRACTOR
#define LIB_TYPE_DIFORTRANTYPEEXTRACTOR

#include <optional>

namespace llvm {
class DIType;
class Type;
}  // namespace llvm

namespace dimeta::dataflow {
struct CallValuePath;
}

namespace dimeta::fortran {
bool is_fortran_descriptor(llvm::Type* type);
std::optional<llvm::DIType*> extract(const dataflow::CallValuePath& call_path, std::optional<llvm::DIType*>);
}  // namespace dimeta::fortran

#endif /* LIB_TYPE_DIFORTRANTYPEEXTRACTOR */
