include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(FeatureSummary)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE STRING "" FORCE)
include(FetchContent)

find_package(LLVM 10 REQUIRED CONFIG)
set_package_properties(LLVM PROPERTIES
  URL https://llvm.org/
  TYPE REQUIRED
  PURPOSE
  "LLVM framework installation required to compile (and apply) TypeART and TyCart."
)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

include(modules/dimeta-llvm)
include(modules/dimeta-llvm-lit)
include(modules/dimeta-format)
include(modules/dimeta-target-util)

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

dimeta_find_llvm_progs(DIMETA_CLANG_EXEC "clang;clang-13;clang-12;clang-11;clang-10" "clang")
dimeta_find_llvm_progs(DIMETA_CLANGCXX_EXEC "clang++;clang-13;clang-12;clang-11;clang++-10" "clang++")
dimeta_find_llvm_progs(DIMETA_LLC_EXEC "llc;llc-13;llc-12;llc-11;llc-10" "llc")
dimeta_find_llvm_progs(DIMETA_OPT_EXEC "opt;opt-13;opt-12;opt-11;opt-10" "opt")
dimeta_find_llvm_progs(DIMETA_FILECHECK_EXEC "FileCheck;FileCheck-13;FileCheck-12;FileCheck-11;FileCheck-10" "FileCheck")

