set(target test_unit)

file(GLOB sources *.cpp)

add_executable(${target} ${sources})
target_compile_options(${target} PRIVATE ${CXX_OPTS_FMA})
target_link_libraries(${target} GTest::GTest GTest::Main)
target_include_directories(${target} PRIVATE ${TFCP_SRC_INCLUDE_DIR})
