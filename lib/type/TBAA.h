//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_TBAA_H
#define DIMETA_TBAA_H

#include <optional>

namespace llvm {
class DIType;
}  // namespace llvm

namespace dimeta::dataflow {
class ValuePath;
}

namespace dimeta::tbaa {

std::optional<llvm::DIType*> resolve_tbaa(llvm::DIType* root, const dataflow::ValuePath& path);

}  // namespace dimeta::tbaa

#endif  // DIMETA_TBAA_H
