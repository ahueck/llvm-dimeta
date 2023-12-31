#include "Dimeta.h"
#include "MetaParse.h"
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

  parser::DITypeParser dimeta_parser;

  assert(type_data.entry_type.has_value() && "Parsing stack type requires entry type.");
  dimeta_parser.traverseType(type_data.entry_type.value());

  const auto result = dimeta_parser.getParsedType();
  return location::LocatedType{result.type_, loc.value()};
}

std::optional<location::LocatedType> located_type_for(const llvm::AllocaInst* ai) {
  auto type_data = type_for(ai);
  if (!type_data) {
    LOG_DEBUG("Could not determine type.");
    return {};
  }
  return located_type_for(type_data.value());
}

std::optional<location::LocatedType> located_type_for(const llvm::CallBase* cb) {
  auto type_data = type_for(cb);
  if (!type_data) {
    LOG_DEBUG("Could not determine type.");
    return {};
  }
  return located_type_for(type_data.value());
}

std::optional<location::LocatedType> located_type_for(const llvm::GlobalVariable* gv) {
  auto type_data = type_for(gv);
  if (!type_data) {
    LOG_DEBUG("Could not determine type.");
    return {};
  }
  return located_type_for(type_data.value());
}

}  // namespace dimeta