function(configure_dimeta_script input output)
    cmake_parse_arguments(
      ARG "" "" "TEST_MODE" ${ARGN}
    )
    set(DIMETA_SCRIPT_DIR ${CMAKE_BINARY_DIR}/scripts)


    if(${LLVM_VERSION_MAJOR} VERSION_GREATER_EQUAL "13")
        set(DIMETA_OPT_ARGS "-enable-new-pm=0")
    endif()

    if(ARG_TEST_MODE)
      set(DIMETA_TEST_MODE 1)
    endif()

    dimeta_target_generate_file(${input} ${output})
endfunction()

configure_dimeta_script(apply.tmpl apply.sh)

configure_dimeta_script(apply.tmpl apply-test.sh
  TEST_MODE ON
)