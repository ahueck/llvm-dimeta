set(LIB_SOURCES
  Dimeta.cpp
  DimetaIO.cpp
  Dimeta.h
  DimetaParse.h
  TBAA.cpp
  DataflowAnalysis.cpp
  GEP.cpp
  SourceLocType.cpp
  DimetaParse.cpp
  DIParser.cpp
  DIVisitorUtil.cpp
)

add_library(dimeta_Types STATIC
  "${LIB_SOURCES}"
)

add_library(dimeta::Types ALIAS dimeta_Types)

dimeta_target_define_file_basename(dimeta_Types)

set_property(TARGET dimeta_Types PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(dimeta_Types
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
         ${PROJECT_SOURCE_DIR}/lib/
)

target_include_directories(dimeta_Types SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})

target_compile_definitions(dimeta_Types PRIVATE $<$<BOOL:${DIMETA_USE_HEAPALLOCSITE}>:DIMETA_USE_HEAPALLOCSITE=1>)

#target_link_libraries(dimeta_Types PRIVATE LLVMCore)

target_compile_definitions(dimeta_Types
        PRIVATE
        LLVM_VERSION_MAJOR=${LLVM_VERSION_MAJOR}
        )

dimeta_target_compile_opts(dimeta_Types)

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
