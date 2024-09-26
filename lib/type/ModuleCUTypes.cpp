#include "Dimeta.h"
#include "DimetaData.h"
#include "DimetaParse.h"

#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"

#include <llvm/Support/Casting.h>

namespace dimeta {

std::optional<CompileUnitTypeList> compile_unit_types(const llvm::Module* module) {
  CompileUnitTypeList list;
  for (auto* compile_unit : module->debug_compile_units()) {
    CompileUnitTypes current_cu;
    current_cu.name = compile_unit->getFilename();
    for (auto* retained_type : compile_unit->getRetainedTypes()) {
      if (auto* type = llvm::dyn_cast<llvm::DIType>(retained_type)) {
        auto dimeta_result = parser::make_dimetadata(type);
        if (!dimeta_result) {
          continue;
        }
        current_cu.types.push_back(dimeta_result->type_);
      }
    }
    list.push_back(current_cu);
  }
  return (list.empty() ? std::optional<CompileUnitTypeList>{} : list);
}

}  // namespace dimeta