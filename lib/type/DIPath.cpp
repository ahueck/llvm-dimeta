//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIPath.h"

#include "llvm/Support/raw_ostream.h"

#include <algorithm>

namespace dimeta::type::dipath {

void ValueToDiPath::emplace_back(const llvm::Value* val, llvm::DIType* mapped_di_type, const std::string reason) {
  path_to_ditype.emplace_back(IRMapping{val, mapped_di_type, std::move(reason)});
}

std::optional<llvm::DIType*> ValueToDiPath::final_type() const {
  if (path_to_ditype.empty()) {
    return {};
  }
  const auto& ditype = path_to_ditype.back();
  return ditype.mapped != nullptr ? std::optional{ditype.mapped} : std::nullopt;
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& os, const ValueToDiPath& vdp) {
#if DIMETA_LOG_LEVEL > 2  // FIXME: For coverage
  const auto& mappings = vdp.path_to_ditype;
  if (mappings.empty()) {
    os << "[]";
    return os;
  }

  os << "[\n";
  for (auto it = mappings.begin(); it != mappings.end(); ++it) {
    const auto& mapping = *it;
    os << "  {\n";
    os << "    IR: ";
    if (mapping.value) {
      mapping.value->print(os, true);
    } else {
      os << "null";
    }
    os << ",\n";

    os << "    DI: ";
    if (mapping.mapped) {
      os << log::ditype_str(mapping.mapped);
    } else {
      os << "null";
    }

    if (!mapping.reason.empty()) {
      os << ",\n";
      os << "    Reason: \"" << mapping.reason << "\"\n";
    } else {
      os << "\n";
    }
    os << "  }";
    if (std::next(it) != mappings.end()) {
      os << ",";
    }
    os << "\n";
  }
  os << "]";
#endif
  return os;
}

}  // namespace dimeta::type::dipath
