#======================================================================
# 2019 (c) Evgeny Latkin
# License: Apache 2.0 (http://www.apache.org/licenses/)
#======================================================================

# Build:
# - tfcp library
# - unit tests
# - examples

cmake_minimum_required(VERSION 3.0)

project(tfcp CXX)

# Local copy:
# - googletest (no googlemock)
set(GTEST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/googletest)

set(TFCP_SOURCE_DIR ${CMAKE_SOURCE_DIR})

# OS and compiler specific options:
# - CXX_OPTS_FMA: enable AVX2 with FMA
#
# Supported compilers:
# - g++, clang++, icc for Linux
# - cl, icl for Windows
if (UNIX AND NOT APPLE)
    set(Linux TRUE)
    set(CXX_OPTS_FMA "-mfma")
elseif (WIN32)
    set(Windows TRUE)
    set(CXX_OPTS_FMA "/arch:AVX2")
else()
    message(FATAL "Unsupported OS: not Linux or Windows")
endif()

# Output folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build tests
add_subdirectory(tests)

# CTest
enable_testing()
add_test(NAME test_unit COMMAND test_unit)
