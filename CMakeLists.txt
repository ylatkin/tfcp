cmake_minimum_required(VERSION 3.0)

project(tfcp)

set(TFCP_INCLUDE_DIR     ${CMAKE_SOURCE_DIR}/include)     # public
set(TFCP_SRC_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/include) # private
set(TFCP_SRC_DIR         ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(tests)
