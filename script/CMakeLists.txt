include(CheckCSourceCompiles)

function(configure_dimeta_script input output)
    cmake_parse_arguments(
      ARG "" "" "TEST_MODE;INSTALL_MODE;COMPILER" ${ARGN}
    )
    set(DIMETA_SCRIPT_DIR ${CMAKE_BINARY_DIR}/scripts)

    set(DIMETA_SCRIPT_DIR ${PROJECT_SOURCE_DIR}/scripts)

    if(ARG_INSTALL_MODE)
        set(DIMETA_INCLUDE_DIRS
                "-I${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
        )
        set(DIMETA_RT_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
        set(DIMETA_PASS_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
        set(DIMETA_ANALYSIS_PASS_DIR
                ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
        )
        set(DIMETA_BINARY_DIR -I${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
        set(DIMETA_RELOCATABLE 1)
    else()
        set(DIMETA_INCLUDE_DIRS
                "-I${PROJECT_SOURCE_DIR}/lib/type"
        )
        if(LIBRARY_OUTPUT_PATH)
            set(DIMETA_RT_DIR ${LIBRARY_OUTPUT_PATH})
            set(DIMETA_PASS_DIR ${LIBRARY_OUTPUT_PATH})
            set(DIMETA_ANALYSIS_PASS_DIR ${LIBRARY_OUTPUT_PATH})
        else()
            set(DIMETA_RT_DIR ${CMAKE_BINARY_DIR}/lib/runtime)
            set(DIMETA_PASS_DIR ${CMAKE_BINARY_DIR}/lib/passes)
            set(DIMETA_ANALYSIS_PASS_DIR ${DIMETA_PASS_DIR}/analysis)
        endif()

        if(EXECUTABLE_OUTPUT_PATH)
            set(DIMETA_BINARY_DIR ${EXECUTABLE_OUTPUT_PATH})
        else()
            set(DIMETA_BINARY_DIR ${CMAKE_BINARY_DIR})
        endif()

        set(DIMETA_RELOCATABLE 0)
    endif()

    if(ARG_COMPILER)
        set(DIMETA_COMPILER ${ARG_COMPILER})
    endif()

    set(DIMETA_OPT "${DIMETA_OPT_EXEC}")
    if(${LLVM_VERSION_MAJOR} VERSION_GREATER_EQUAL "13")
        set(DIMETA_OPT "${DIMETA_OPT} -enable-new-pm=0")
    endif()

    set(DIMETA_LLC "${DIMETA_LLC_EXEC}")

    if(ARG_TEST_MODE)
      set(DIMETA_TEST_MODE 1)
    endif()

    dimeta_target_generate_file(${input} ${output})
endfunction()

configure_dimeta_script(apply.tmpl apply.sh)

configure_dimeta_script(apply.tmpl apply-test.sh
  TEST_MODE ON
)
configure_dimeta_script(
        dimeta-wrapper.in dimeta-clang${CMAKE_DEBUG_POSTFIX}
        INSTALL_MODE ON
        COMPILER ${DIMETA_CLANG_EXEC}
)
configure_dimeta_script(
        dimeta-wrapper.in dimeta-clang++${CMAKE_DEBUG_POSTFIX}
        INSTALL_MODE ON
        COMPILER ${DIMETA_CLANGCXX_EXEC}
)

configure_dimeta_script(
        dimeta-wrapper.in dimeta-clang-test
        COMPILER ${DIMETA_CLANG_EXEC}
)
configure_dimeta_script(
        dimeta-wrapper.in dimeta-clang++-test
        COMPILER ${DIMETA_CLANGCXX_EXEC}
)

function(dimeta_find_mpi_vendor_helper symbol ret_value)
    find_package(MPI)

    if(NOT MPI_FOUND)
        set(${ret_value} false)
        return()
    endif()

    set(MPI_TEST_CODE
    "#include <mpi.h>
    int main(void) {
      #if !defined(${symbol})
        an_illegal_stmt
      #endif
      return 0;
    }")

    set(CMAKE_REQUIRED_INCLUDES ${MPI_C_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_FLAGS "${MPI_C_COMPILE_FLAGS}")
    set(CMAKE_REQUIRED_DEFINITIONS "")
    set(CMAKE_REQUIRED_LIBRARIES "${MPI_C_LIBRARIES}")

    check_c_source_compiles("${MPI_TEST_CODE}" ${ret_value})
endfunction()

function(set_dimeta_mpi_compiler)
    # Note on mpich & Intel: Intel may also define \"MPICH_NAME\"
    # in mpi.h, so check MPICH last
    list(APPEND l_vendor OPEN_MPI INTEL_MPI MPICH)
    list(APPEND l_symbol OPEN_MPI I_MPI_VERSION MPICH_NAME)
    list(APPEND l_env_c OMPI_CC I_MPI_CC MPICH_CC)
    list(APPEND l_env_cxx OMPI_CXX I_MPI_CXX MPICH_CXX)

    foreach(vendor symbol env_c env_cxx IN ZIP_LISTS l_vendor l_symbol l_env_c l_env_cxx)
        dimeta_find_mpi_vendor_helper(${symbol} DIMETA_HAVE_${vendor})
        if(DIMETA_HAVE_${vendor})
            if(MPI_C_FOUND)
                set(DIMETA_MPICC
                        "env ${env_c}=${DIMETA_CLANG_EXEC} ${MPI_C_COMPILER}"
                        CACHE STRING "TypeART MPICC compiler command for scripts"
                )
                mark_as_advanced(DIMETA_MPICC)
            endif()
            if(MPI_CXX_FOUND)
                set(DIMETA_MPICXX
                        "env ${env_cxx}=${DIMETA_CLANGCXX_EXEC} ${MPI_CXX_COMPILER}"
                        CACHE STRING "TypeART MPICXX compiler command for scripts"
                )
                mark_as_advanced(DIMETA_MPICXX)
            endif()
            break()
        endif()
    endforeach()
endfunction()

find_package(MPI)
if(MPI_FOUND)
    set_dimeta_mpi_compiler()
endif()

if(MPI_C_FOUND)
    configure_dimeta_script(
            dimeta-wrapper.in dimeta-mpicc${CMAKE_DEBUG_POSTFIX}
            INSTALL_MODE ON
            WITH_FILTER ON
            COMPILER "${DIMETA_MPICC}"
    )
    configure_dimeta_script(
            dimeta-wrapper.in dimeta-mpicc-test
            WITH_FILTER ON
            COMPILER "${DIMETA_MPICC}"
    )
endif()

if(MPI_CXX_FOUND)
    configure_dimeta_script(
            dimeta-wrapper.in dimeta-mpic++${CMAKE_DEBUG_POSTFIX}
            INSTALL_MODE ON
            WITH_FILTER ON
            COMPILER "${DIMETA_MPICXX}"
    )
    configure_dimeta_script(
            dimeta-wrapper.in dimeta-mpic++-test
            WITH_FILTER ON
            COMPILER "${DIMETA_MPICXX}"
    )
endif()

set(DIMETA_INSTALL_SCRIPT_CANDIDATE
        dimeta-mpicc
        dimeta-mpic++
        dimeta-clang++
        dimeta-clang
        CACHE INTERNAL
        "Names of Dimeta scripts which can be installed."
)

install(
        PROGRAMS
        ${CMAKE_CURRENT_BINARY_DIR}/dimeta-clang++${CMAKE_DEBUG_POSTFIX}
        ${CMAKE_CURRENT_BINARY_DIR}/dimeta-clang${CMAKE_DEBUG_POSTFIX}
        $<$<AND:$<BOOL:${MPI_C_FOUND}>,$<BOOL:${DIMETA_MPI_WRAPPER}>>:${CMAKE_CURRENT_BINARY_DIR}/dimeta-mpicc${CMAKE_DEBUG_POSTFIX}>
        $<$<AND:$<BOOL:${MPI_CXX_FOUND}>,$<BOOL:${DIMETA_MPI_WRAPPER}>>:${CMAKE_CURRENT_BINARY_DIR}/dimeta-mpic++${CMAKE_DEBUG_POSTFIX}>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
)