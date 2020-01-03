cmake_minimum_required(VERSION 3.0)

project(tfcp)

set(TFCP_INCLUDE_DIR     ${CMAKE_SOURCE_DIR}/include)     # public
set(TFCP_SRC_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/include) # private
set(TFCP_SRC_DIR         ${CMAKE_SOURCE_DIR}/src)

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
