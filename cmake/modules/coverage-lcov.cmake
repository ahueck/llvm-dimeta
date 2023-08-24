find_program(DIMETA_LCOV_EXEC lcov)
find_program(DIMETA_GENHTML_EXEC genhtml)

if(DIMETA_LCOV_EXEC-NOTFOUND OR DIMETA_GENHTML_EXEC-NOTFOUND)
    message(WARNING "lcov and genhtml command needed for coverage.")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    dimeta_find_llvm_progs(DIMETA_LLVMCOV_EXEC "llvm-cov-${LLVM_VERSION_MAJOR};llvm-cov")
    if(NOT DIMETA_LLVMCOV_EXEC)
        message(FATAL_ERROR "Did not find llvm-cov, which is required for Clang-based LCOV coverage.")
    endif()
    # workaround lcov and clang --coverage have a version mismatch
    file(
        GENERATE
        OUTPUT ${CMAKE_BINARY_DIR}/script/llvm-gcov.sh
        CONTENT "#!/usr/bin/env bash\n\n${DIMETA_LLVMCOV_EXEC} gcov \"$@\"\n"
        FILE_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ
        WORLD_READ
    )
    set(GCOV_TOOL --gcov-tool ${CMAKE_BINARY_DIR}/script/llvm-gcov.sh)
endif()

set(
    DIMETA_COVERAGE_TRACE_COMMAND
    ${DIMETA_LCOV_EXEC} ${GCOV_TOOL} -c -q
    -o "${PROJECT_BINARY_DIR}/coverage.info"
    -d "${PROJECT_BINARY_DIR}"
    --include "${PROJECT_SOURCE_DIR}/*"
    CACHE STRING
    "; Command to generate a trace for the 'dimeta_coverage' target"
)

set(
    DIMETA_COVERAGE_HTML_COMMAND
    ${DIMETA_GENHTML_EXEC} --legend -f -q
    "${PROJECT_BINARY_DIR}/coverage.info"
    -p "${PROJECT_SOURCE_DIR}"
    -o "${PROJECT_BINARY_DIR}/coverage_html"
    CACHE STRING
    "; Command to generate an HTML report for the 'dimeta_coverage' target"
)

add_custom_target(
    dimeta-coverage
    COMMAND ${DIMETA_COVERAGE_TRACE_COMMAND}
    COMMAND ${DIMETA_COVERAGE_HTML_COMMAND}
    COMMENT "Generating dimeta coverage report"
    VERBATIM
)

add_custom_target(
    dimeta-coverage-clean
    COMMAND ${DIMETA_LCOV_EXEC} -d ${CMAKE_BINARY_DIR} -z
)
