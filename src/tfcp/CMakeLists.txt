# Build:
# - TFCP library (static)

set(TARGET tfcp)

file(GLOB SOURCES *.cpp)

add_library(${TARGET} STATIC ${SOURCES})

target_compile_options(${TARGET} PRIVATE ${CXX_OPTS_FMA}
                                         ${CXX_OPTS_FP})

target_include_directories(${TARGET} PRIVATE ${TFCP_SOURCE_DIR}/include
                                             ${TFCP_SOURCE_DIR}/src/include)
