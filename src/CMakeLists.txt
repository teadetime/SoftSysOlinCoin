cmake_minimum_required(VERSION 3.13)
include(ExternalProject)
# set the project name
project(OlinCoin C)

# set(MBED_PROJECT_NAME "mbed_tls")
# set(EXTERN_LIB_NAME "extern_lib")


# include(FetchContent)
# set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/../bin)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mbed_tests/)
# include(FetchContent)
# FetchContent_Declare(MbedTLS
# GIT_REPOSITORY https://github.com/ARMmbed/mbedtls.git
# GIT_TAG v3.1.0
# # SOURCE_DIR    ${CMAKE_BINARY_DIR}/../external/src
# # BINARY_DIR    ${CMAKE_BINARY_DIR}/../external/bin
# )
# FetchContent_MakeAvailable(MbedTLS)


set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
message(STATUS "root cmake Source: ${CMAKE_SOURCE_DIR}")
message(STATUS "root Cmake/Bin?: ${CMAKE_BINARY_DIR}")
message(STATUS "root Cmake Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_subdirectory(external)
add_executable(mbed ${CMAKE_SOURCE_DIR}/src/tests/core/blocks/mbed_test.c)


#find_package(MbedTLS REQUIRED)
target_link_libraries(mbed PUBLIC mbedtls
mbedcrypto
mbedx509)


