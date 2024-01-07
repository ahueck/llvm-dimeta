//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "Dimeta.h"
#include "DimetaData.h"
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
    const auto* global_var    = *gv;
    const auto file           = std::string{global_var->getFilename()};
    const auto function_scope = [](const auto global) -> std::string {
      const auto* scope = global->getScope();
      if (scope) {
        return std::string{scope->getName()};
      }
      return "";
    }(global_var);
    return location::SourceLocation{file,            //
                                    function_scope,  //
                                    global_var->getLine()};
  }

  return {};
}

std::optional<LocatedType> located_type_for(const DimetaData& type_data) {
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
  return LocatedType{dimeta_result->type_, loc.value()};
}

template <typename IRNode>
std::optional<LocatedType> get_located_type(const IRNode* node) {
  auto type_data = type_for(node);
  if (!type_data) {
    LOG_DEBUG("Could not determine type.");
    return {};
  }
  return located_type_for(type_data.value());
}

std::optional<LocatedType> located_type_for(const llvm::AllocaInst* ai) {
  return get_located_type(ai);
}

std::optional<LocatedType> located_type_for(const llvm::CallBase* cb) {
  return get_located_type(cb);
}

std::optional<LocatedType> located_type_for(const llvm::GlobalVariable* gv) {
  return get_located_type(gv);
}

}  // namespace dimeta