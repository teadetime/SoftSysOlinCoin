cmake_minimum_required(VERSION 3.13)
include(ExternalProject)
# set the project name
project(OlinCoin C)

set(MBED_PROJECT_NAME "mbed_tls")
set(EXTERN_LIB_NAME "extern_lib")

# ExternalProject_Add(test
#   GIT_REPOSITORY https://github.com/ARMmbed/mbedtls.git
#   GIT_TAG v3.1.0
#   UPDATE_DISCONNECTED true
#   PREFIX ${CMAKE_CURRENT_BINARY_DIR}/${EXTERN_LIB_NAME}
#   CONFIGURE_COMMAND 
#   STEP_TARGETS build
#   BUILD_COMMAND 
#   INSTALL_COMMAND
# )

include(FetchContent)
FetchContent_Declare(MbedTLS
GIT_REPOSITORY https://github.com/ARMmbed/mbedtls.git
GIT_TAG v3.1.0
SOURCE_DIR    ${CMAKE_BINARY_DIR}/../external/src
BINARY_DIR    ${CMAKE_BINARY_DIR}/../external/bin
)

# ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
FetchContent_MakeAvailable(MbedTLS)
# add the executable
# add_executable(OlinCoin
#   ${CMAKE_SOURCE_DIR}/tests/core/txs/test_base_tx.c 
#   ${CMAKE_SOURCE_DIR}/core/txs/base_tx.c)


# target_include_directories(OlinCoin PUBLIC
#   "${PROJECT_BINARY_DIR}/includes/blocks"
#   "${PROJECT_BINARY_DIR}/includes/txs"
#   "${PROJECT_BINARY_DIR}/includes/globals"
#   )
# target_compile_options(OlinCoin PRIVATE  -Wall -Wextra -pedantic) #-O2)

#set(MbedTLS_DIR "${CMAKE_BINARY_DIR}/${EXTERN_LIB_NAME}/src/${MBED_PROJECT_NAME}/cmake")
#message(STATUS "Looking for mbed config: ${MbedTLS_DIR}")
#ind_library(MBED_LIB mbedtls REQUIRED HINTS "/home/nathan/Downloads/build_test/library")

set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/../our_files)
add_executable(mbed ${CMAKE_SOURCE_DIR}/tests/core/blocks/mbed_test.c)


#find_package(MbedTLS REQUIRED)
target_link_libraries(mbed PUBLIC mbedtls
mbedcrypto
mbedx509)

message(STATUS "mbed_found?: ${MbedTLS_FOUND}")
