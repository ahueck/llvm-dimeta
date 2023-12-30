#include "Dimeta.h"

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

}  // namespace dimeta