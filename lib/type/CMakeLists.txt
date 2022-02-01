set(LIB_SOURCES
  Dimeta.cpp
)

add_library(dimeta_Types
  "${LIB_SOURCES}"
)

add_library(dimeta::Types ALIAS dimeta_Types)

target_include_directories(dimeta_Types
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
         ${PROJECT_SOURCE_DIR}/lib/
)

target_include_directories(dimeta_Types SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})

#target_compile_definitions(dimeta_Types PRIVATE LLVM_ENABLE_DUMP)

target_link_libraries(dimeta_Types PRIVATE LLVMCore)

target_include_directories(
  dimeta_Types
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set_target_properties(
  dimeta_Types
  PROPERTIES
  OUTPUT_NAME "dimetaTypes"
  EXPORT_NAME "Types"
)
