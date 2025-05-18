cmake_minimum_required(VERSION 3.20)

project(dimeta
  VERSION 0.3.0
  HOMEPAGE_URL https://github.com/ahueck/llvm-dimeta
  LANGUAGES C CXX
)

list(
  APPEND
  CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(dimetaToolchain)

if(DIMETA_IS_TOP_LEVEL)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_VERBOSE_MAKEFILE OFF)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

dimeta_target_format(
  format-dimeta-sources "Formats project source files"
  TARGETS lib/*/*.cpp
          lib/*/*.h
)

add_subdirectory(lib)

if(DIMETA_IS_TOP_LEVEL)
  enable_testing()
  add_subdirectory(script)
  add_subdirectory(test)
endif()
