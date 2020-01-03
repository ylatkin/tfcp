#======================================================================
# 2019 (c) Evgeny Latkin
# License: Apache 2.0 (http://www.apache.org/licenses/)
#======================================================================

# Build:
# - gtest and gtest_main libraries

add_library(gtest      STATIC ${GTEST_SOURCE_DIR}/src/gtest-all.cc)
add_library(gtest_main STATIC ${GTEST_SOURCE_DIR}/src/gtest_main.cc)

target_include_directories(gtest      PRIVATE ${GTEST_SOURCE_DIR} ${GTEST_SOURCE_DIR}/include)
target_include_directories(gtest_main PRIVATE ${GTEST_SOURCE_DIR} ${GTEST_SOURCE_DIR}/include)

target_compile_options(gtest      PRIVATE "-DGTEST_HAS_PTHREAD=0")
target_compile_options(gtest_main PRIVATE "-DGTEST_HAS_PTHREAD=0")

# Add tests:

add_subdirectory(unit)
