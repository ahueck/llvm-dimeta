//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
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

  const auto make_source_loc = [](const auto* variable) {
    const auto file           = std::string{variable->getFilename()};
    const auto function_scope = [](const auto alloc) -> std::string {
      const auto* scope = alloc->getScope();
      if (scope) {
        return std::string{scope->getName()};
      }
      return "";
    }(variable);
    return location::SourceLocation{file,            //
                                    function_scope,  //
                                    variable->getLine()};
  };

  if (const auto gv = std::get_if<llvm::DIGlobalVariable*>(&data.di_variable.value())) {
    const auto* global_var = *gv;
    return make_source_loc(global_var);
  }

  if (const auto alloc_var = std::get_if<llvm::DILocalVariable*>(&data.di_variable.value())) {
    const auto* alloc = *alloc_var;
    return make_source_loc(alloc);
  }

  return {};
}

std::optional<LocatedType> located_type_for(const DimetaData& type_data) {
  auto loc = location_for(type_data);
  if (!loc) {
    LOG_DEBUG("Could not determine source location.");
    return {};
  }

  if (!type_data.entry_type) {
    LOG_DEBUG("Could not determine type (missing entry type).");
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