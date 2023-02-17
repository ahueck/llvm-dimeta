function(configure_dimeta_script input output)
    set(DIMETA_SCRIPT_DIR ${CMAKE_BINARY_DIR}/scripts)


    if(${LLVM_VERSION_MAJOR} VERSION_GREATER_EQUAL "13")
        set(DIMETA_OPT_ARGS "-enable-new-pm=0")
    endif()

    dimeta_target_generate_file(${input} ${output})
endfunction()

configure_dimeta_script(apply.tmpl apply.sh)