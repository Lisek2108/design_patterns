cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(DesignPatterns LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

option(DP_ENABLE_TESTS "Enable tests for built targets" OFF)
option(DP_USE_CONAN "Use conan package manager for third party libraries" OFF)

set(DP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/patterns)

if(DP_USE_CONAN)
  include(cmake/conan.cmake)
  run_conan()
endif()

if(DP_ENABLE_TESTS)
  enable_testing()
  find_package(catch2 QUIET)
  if(NOT Catch2_FOUND)
    set(DP_TESTS_ENABLED FALSE)
    message(
      WARNING
        "Testing was enabled but Catch2 was not found. Tests won't be built")
  else()
    set(DP_TESTS_ENABLED TRUE)
  endif()
endif()

add_subdirectory(patterns)
