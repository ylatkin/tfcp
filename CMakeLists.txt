# Build:
# - tfcp library
# - unit tests
# - examples

cmake_minimum_required(VERSION 3.0)

project(TFCP CXX)

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
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
if (UNIX AND NOT APPLE)
    set(Linux TRUE)
    message(STATUS "OS: Linux")
    set(CXX_OPTS_FMA "-mfma")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
        set(CXX_OPTS_FP "-fp-model=precise")
    else()
        set(CXX_OPTS_FP "")
    endif()
elseif (WIN32)
    set(Windows TRUE)
    message(STATUS "OS: Windows")
    set(CXX_OPTS_FMA "/arch:AVX2")
    set(CXX_OPTS_FP  "/fp:precise")
else()
    message(FATAL "Unsupported OS: not Linux or Windows")
endif()

# Output folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build library
add_subdirectory(src)

# Build tests
add_subdirectory(tests)

# CTest
enable_testing()
add_test(NAME test_unit COMMAND test_unit)
