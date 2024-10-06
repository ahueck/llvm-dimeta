//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIROOTTYPE_H
#define DIMETA_DIROOTTYPE_H

#include <optional>

namespace dimeta::dataflow {
struct ValuePath;
}  // namespace dimeta::dataflow

namespace llvm {
class DIType;
}

namespace dimeta::root { 

std::optional<llvm::DIType*> find_type_root(const dataflow::ValuePath& path);

}

#endif
