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


function(configure_dimeta_script input output)
    if(${LLVM_VERSION_MAJOR} VERSION_GREATER_EQUAL "13")
      set(NEW_PM_REQUIRED 1)
    else()
      set(NEW_PM_REQUIRED 0)
    endif()
    set(OPT_SHIM ${PROJECT_BINARY_DIR}/test/verifier/opt-shim.sh)
    set(TESTPASS_PASS_DIR ${CMAKE_BINARY_DIR}/test/verifier)

    dimeta_target_generate_file(${input} ${output})
endfunction()

configure_dimeta_script(opt-shim.in opt-shim.sh)

configure_dimeta_script(reduce-tester.in reduce-tester.sh)
