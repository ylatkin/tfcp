# Build:
# - unit_tests application
#
# Added to CTest at top-level CMake

set(TARGET test_unit)

file(GLOB SOURCES *.cpp)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} tfcp gtest gtest_main)

target_compile_options(${TARGET} PRIVATE ${CXX_OPTS_FMA}
                                         ${CXX_OPTS_FP})

target_include_directories(${TARGET} PRIVATE ${TFCP_SOURCE_DIR}/include
                                             ${TFCP_SOURCE_DIR}/src/include
                                             ${TFCP_SOURCE_DIR}/tests/include
                                             ${GTEST_SOURCE_DIR}/googletest/include)
