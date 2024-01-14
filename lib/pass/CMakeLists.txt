set(PASS_SOURCES
  DimetaPass.cpp
)

dimeta_llvm_module(dimeta_AnalysisPass
  "${PASS_SOURCES}"
  INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/lib/type
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

target_link_libraries(dimeta_AnalysisPass PRIVATE dimeta::Types)

set(CONFIG_NAME dimetaAnalysisPass)
set(TARGETS_EXPORT_NAME ${CONFIG_NAME}Targets)

install(
        TARGETS dimeta_AnalysisPass
        EXPORT ${TARGETS_EXPORT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
        EXPORT ${TARGETS_EXPORT_NAME}
        NAMESPACE dimeta::
        DESTINATION ${DIMETA_INSTALL_CONFIGDIR}
)

export(
        EXPORT ${TARGETS_EXPORT_NAME}
        FILE ${CMAKE_BINARY_DIR}/${TARGETS_EXPORT_NAME}.cmake
        NAMESPACE dimeta::
)
