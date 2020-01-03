#======================================================================
# 2019 (c) Evgeny Latkin
# License: Apache 2.0 (http://www.apache.org/licenses/)
#======================================================================

# Build:
# - unit_tests application
#
# NB: added to CTest at top-level CMake

file(GLOB sources *.cpp)

add_executable(test_unit ${sources})

target_link_libraries(test_unit gtest gtest_main)

target_compile_options(test_unit PRIVATE ${CXX_OPTS_FMA})

target_include_directories(test_unit PRIVATE ${TFCP_SOURCE_DIR}/src/include
                                             ${GTEST_SOURCE_DIR}/include)
