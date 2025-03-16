# The Diu Programming Language.
# Copyright (c) 2024, Diu
# https://furzoom.com
# https://github.com/diu-lang/diu
#
# MIT License

cmake_minimum_required(VERSION 3.9)
project(diu VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR};${CMAKE_SOURCE_DIR}/scripts/cmake")
include(config)

# This project can use C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

OPTION(DIU_INSTALL "Install diu's header and library" ON)
OPTION(DIU_BUILD_TESTS "Build diu's unit tests" ON)
OPTION(DIU_BUILD_TESTS_COVERAGE "Build diu's test coverage" OFF)

set(C_FLAGS
    ${CXX_FLAGS}
    -g
    -Wall
    -Wextra
    -Werror
    -Wno-unused-parameter
)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    list(APPEND C_FLAGS "-Wthread-safety")
endif()

string(REPLACE ";" " " CMAKE_C_FLAGS "${C_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG "-O0 -D_DEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

if(DIU_BUILD_TESTS_COVERAGE)
    set(BUILD_TESTS ON)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif()

message(STATUS "C_FLAGS = " ${CMAKE_C_FLAGS} " " ${CMAKE_C_FLAGS_${BUILD_TYPE}})

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}"
)

add_subdirectory(src)
