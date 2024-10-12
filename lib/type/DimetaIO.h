//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIMETAIO_H
#define DIMETA_DIMETAIO_H

#include "DimetaData.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

namespace dimeta::io {

bool emit(llvm::raw_string_ostream& oss, const QualifiedFundamental& compound);
bool input(llvm::StringRef yaml, QualifiedFundamental& compound);
bool emit(llvm::raw_string_ostream& oss, const QualifiedCompound& compound);
bool input(llvm::StringRef yaml, QualifiedCompound& compound);

bool emit(llvm::raw_string_ostream& oss, const LocatedType& type);
bool input(llvm::StringRef yaml, LocatedType& compound);

bool emit(llvm::raw_string_ostream& oss, const CompileUnitTypes& cu_types);
bool input(llvm::StringRef yaml, CompileUnitTypes& cu_types);

bool emit(llvm::raw_string_ostream& oss, const CompileUnitTypeList& list);
bool input(llvm::StringRef yaml, CompileUnitTypeList& list);

}  // namespace dimeta::io

#endif  // DIMETA_DIMETAIO_H
