# Build:
# - gtest and gtest_main libraries

add_library(gtest      STATIC ${GTEST_SOURCE_DIR}/googletest/src/gtest-all.cc)
add_library(gtest_main STATIC ${GTEST_SOURCE_DIR}/googletest/src/gtest_main.cc)

target_include_directories(gtest      PRIVATE ${GTEST_SOURCE_DIR}/googletest
                                              ${GTEST_SOURCE_DIR}/googletest/include)
target_include_directories(gtest_main PRIVATE ${GTEST_SOURCE_DIR}/googletest
                                              ${GTEST_SOURCE_DIR}/googletest/include)

target_compile_options(gtest      PRIVATE "-DGTEST_HAS_PTHREAD=0")
target_compile_options(gtest_main PRIVATE "-DGTEST_HAS_PTHREAD=0")

# Add tests:

add_subdirectory(unit)
