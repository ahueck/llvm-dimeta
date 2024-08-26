include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
#set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE STRING "" FORCE)
#include(FetchContent)

find_package(LLVM CONFIG HINTS "${LLVM_DIR}")
if(NOT LLVM_FOUND)
  message(STATUS "LLVM not found at: ${LLVM_DIR}.")
  find_package(LLVM REQUIRED CONFIG)
endif()

set_package_properties(LLVM PROPERTIES
  URL https://llvm.org/
  TYPE REQUIRED
  PURPOSE
  "LLVM framework installation required to compile (and apply) project dimeta."
)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
  PROJECT_IS_TOP_LEVEL
)

option(DIMETA_USE_HEAPALLOCSITE "Use heapallocsite metadata for C++ heap allocations." ON)

option(DIMETA_TEST_CONFIGURE_IDE "Add targets for tests to help the IDE with completion etc." ON)
mark_as_advanced(DIMETA_TEST_CONFIGURE_IDE)
option(DIMETA_CONFIG_DIR_IS_SHARE "Install to \"share/cmake/\" instead of \"lib/cmake/\"" OFF)
mark_as_advanced(DIMETA_CONFIG_DIR_IS_SHARE)

option(DIMETA_ENABLE_COVERAGE "Enable coverage targets" OFF)


option(DIMETA_TEST_CONFIG "Set logging levels to appropriate levels for test runner to succeed" OFF)

if(DIMETA_TEST_CONFIG)
  set(DIMETA_LOG_LEVEL 3 CACHE STRING "" FORCE)
else()
  set(DIMETA_LOG_LEVEL 1 CACHE STRING "" FORCE)
endif()

set(warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
  option(
      DIMETA_INCLUDES_WITH_SYSTEM
      "Use SYSTEM modifier for dimeta includes to disable warnings."
      ON
  )
  mark_as_advanced(DIMETA_INCLUDES_WITH_SYSTEM)

  if(DIMETA_INCLUDES_WITH_SYSTEM)
    set(warning_guard SYSTEM)
  endif()
endif()

include(modules/dimeta-llvm)
include(modules/dimeta-format)
include(modules/dimeta-target-util)
if(DIMETA_ENABLE_COVERAGE)
  include(modules/coverage-lcov)
endif()

dimeta_find_llvm_progs(DIMETA_CLANG_EXEC "clang-${LLVM_VERSION_MAJOR};clang" DEFAULT_EXE "clang")
dimeta_find_llvm_progs(DIMETA_CLANGCXX_EXEC "clang++-${LLVM_VERSION_MAJOR};clang++" DEFAULT_EXE "clang++")
dimeta_find_llvm_progs(DIMETA_LLC_EXEC "llc-${LLVM_VERSION_MAJOR};llc" DEFAULT_EXE "llc")
dimeta_find_llvm_progs(DIMETA_OPT_EXEC "opt-${LLVM_VERSION_MAJOR};opt" DEFAULT_EXE "opt")

if(PROJECT_IS_TOP_LEVEL)
  if(NOT CMAKE_BUILD_TYPE)
    # set default build type
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    message(STATUS "Building as debug (default)")
  endif()

  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # set default install path
    set(CMAKE_INSTALL_PREFIX
        "${dimeta_SOURCE_DIR}/install/dimeta"
        CACHE PATH "Default install path" FORCE
    )
    message(STATUS "Installing to (default): ${CMAKE_INSTALL_PREFIX}")
  endif()

    # DIMETA_DEBUG_POSTFIX is only used for Config
    if(CMAKE_DEBUG_POSTFIX)
      set(DIMETA_DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    else()
      set(DIMETA_DEBUG_POSTFIX "-d")
    endif()

  if(NOT CMAKE_DEBUG_POSTFIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_DEBUG_POSTFIX ${DIMETA_DEBUG_POSTFIX})
  endif()
else()
  set(DIMETA_DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
endif()

include(GNUInstallDirs)

set(DIMETA_PREFIX ${PROJECT_NAME})
set(TARGETS_EXPORT_NAME ${DIMETA_PREFIX}Targets)

if(DIMETA_CONFIG_DIR_IS_SHARE)
  set(DIMETA_INSTALL_CONFIGDIR ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME})
else()
  set(DIMETA_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif()
