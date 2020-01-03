cmake_minimum_required(VERSION 3.0)

project(tfcp CXX)

set(TFCP_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(TFCP_INCLUDE_DIR     ${TFCP_SOURCE_DIR}/include)     # public
set(TFCP_SRC_INCLUDE_DIR ${TFCP_SOURCE_DIR}/src/include) # private
set(TFCP_SRC_DIR         ${TFCP_SOURCE_DIR}/src)

set(GTEST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/googletest)

if (UNIX AND NOT APPLE)
    message(STATUS "OS: Linux")
    set(Linux TRUE)
    set(CXX_OPTS_FMA "-mfma")
elseif (WIN32)
    message(STATUS "OS: Windows")
    set(Windows TRUE)
    set(CXX_OPTS_FMA "/arch:AVX2")
else()
    message(FATAL "Unsupported OS: not Linux or Windows")
endif()

add_subdirectory(tests)
