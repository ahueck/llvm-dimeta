//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

//
// Created by ahueck on 28.02.23.
//

#ifndef DIMETA_METAIO_H
#define DIMETA_METAIO_H

#include "DimetaData.h"

#include "llvm/Support/raw_ostream.h"

namespace dimeta {
namespace io {
bool emit(llvm::raw_string_ostream& oss, const QualifiedFundamental& compound);
bool input(llvm::StringRef yaml, QualifiedFundamental& compound);
bool emit(llvm::raw_string_ostream& oss, const QualifiedCompound& compound);
bool input(llvm::StringRef yaml, QualifiedCompound& compound);

bool emit(llvm::raw_string_ostream& oss, const location::LocatedType& type);
bool input(llvm::StringRef yaml, location::LocatedType& compound);
}  // namespace io
}  // namespace dimeta

#endif  // DIMETA_METAIO_H
