set(LIB_SOURCES TestPass.cpp)

dimeta_llvm_module(dimeta_TestPass
  "${LIB_SOURCES}"
  INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/lib/type
)

mark_as_advanced(LLVM_DIMETA_TESTPASS_LINK_INTO_TOOLS)

set_target_properties(
        dimeta_TestPass
        PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}TestPass"
        EXPORT_NAME "TestPass"
)

add_library(dimeta::TestPass ALIAS dimeta_TestPass)

target_include_directories(dimeta_TestPass
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
          ${PROJECT_SOURCE_DIR}/lib/
)

target_include_directories(dimeta_TestPass
        SYSTEM
        PRIVATE
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(dimeta_TestPass PRIVATE dimeta::Types)
