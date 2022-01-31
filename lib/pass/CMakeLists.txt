set(PASS_SOURCES
  DimetaPass.cpp
)

dimeta_llvm_module(dimeta_AnalysisPass
  "${PASS_SOURCES}"
)

mark_as_advanced(LLVM_DIMETA_ANALYSISPASS_LINK_INTO_TOOLS)

set_target_properties(
  dimeta_AnalysisPass
  PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}AnalysisPass"
  EXPORT_NAME "AnalysisPass"
)

add_library(dimeta::AnalysisPass ALIAS dimeta_AnalysisPass)

target_include_directories(dimeta_AnalysisPass
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
         ${PROJECT_SOURCE_DIR}/lib/
)

