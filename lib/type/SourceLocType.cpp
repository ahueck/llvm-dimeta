//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "Dimeta.h"
#include "DimetaParse.h"
#include "support/Logger.h"

#include "llvm/IR/DebugInfoMetadata.h"

#include <optional>

namespace dimeta {

std::optional<location::SourceLocation> location_for(const DimetaData& data) {
  if (data.di_location) {
    auto loc = data.di_location.value();
    return location::SourceLocation{std::string{loc->getFilename()},          //
                                    std::string{loc->getScope()->getName()},  //
                                    loc->getLine()};
  }
  if (!data.di_variable) {
    return {};
  }

  if (const auto gv = std::get_if<llvm::DIGlobalVariable*>(&data.di_variable.value())) {
    const auto* global_var = *gv;
    return location::SourceLocation{std::string{global_var->getFilename()},          //
                                    std::string{global_var->getScope()->getName()},  //
                                    global_var->getLine()};
  }

  return {};
}

std::optional<location::LocatedType> located_type_for(const DimetaData& type_data) {
  auto loc = location_for(type_data);
  if (!loc) {
    LOG_DEBUG("Could not determine source location.");
    return {};
  }

  assert(type_data.entry_type.has_value() && "Parsing stack type requires entry type.");
  auto dimeta_result = parser::make_dimetadata(type_data.entry_type.value());
  if (!dimeta_result) {
    return {};
  }
  return location::LocatedType{dimeta_result->type_, loc.value()};
}

template <typename IRNode>
std::optional<location::LocatedType> get_located_type(const IRNode* node) {
  auto type_data = type_for(node);
  if (!type_data) {
    LOG_DEBUG("Could not determine type.");
    return {};
  }
  return located_type_for(type_data.value());
}

std::optional<location::LocatedType> located_type_for(const llvm::AllocaInst* ai) {
  return get_located_type(ai);
}

std::optional<location::LocatedType> located_type_for(const llvm::CallBase* cb) {
  return get_located_type(cb);
}

std::optional<location::LocatedType> located_type_for(const llvm::GlobalVariable* gv) {
  return get_located_type(gv);
}

}  // namespace dimeta