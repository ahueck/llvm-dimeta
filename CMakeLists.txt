cmake_minimum_required(VERSION 3.20)

project(dimeta
  VERSION 0.1
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(
  APPEND
  CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(dimetaToolchain)

dimeta_target_format(
  format-dimeta-sources "Formats project source files"
  TARGETS lib/pass/*.cpp
          lib/pass/*.h
)

add_subdirectory(lib)
add_subdirectory(script)

enable_testing()
add_subdirectory(test)
