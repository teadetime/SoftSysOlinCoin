cmake_minimum_required(VERSION 3.13)

# set the project name
project(OlinCoin C)

# ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# add the executable
# add_executable(OlinCoin
#   ${CMAKE_SOURCE_DIR}/tests/core/txs/test_base_tx.c 
#   ${CMAKE_SOURCE_DIR}/core/txs/base_tx.c)


# target_include_directories(OlinCoin PUBLIC
#   "${PROJECT_BINARY_DIR}/includes/blocks"
#   "${PROJECT_BINARY_DIR}/includes/txs"
#   "${PROJECT_BINARY_DIR}/includes/globals"
#   )

set(MbedTLS_DIR "/home/nathan/Downloads/build_test/cmake")

find_package(MbedTLS REQUIRED)

#ind_library(MBED_LIB mbedtls REQUIRED HINTS "/home/nathan/Downloads/build_test/library")

add_executable(mbed ${CMAKE_SOURCE_DIR}/tests/core/blocks/mbed_test.c)
target_include_directories(mbed PUBLIC
#"/home/nathan/Downloads/build_test/include/"
# "${PROJECT_BINARY_DIR}/includes/blocks"
# "${PROJECT_BINARY_DIR}/includes/txs"
# "${PROJECT_BINARY_DIR}/includes/globals"
)
target_link_libraries(mbed PUBLIC MbedTLS::mbedtls
  MbedTLS::mbedcrypto
  MbedTLS::mbedx509)
message(STATUS "mbed_found?: ${MbedTLS_FOUND}")

# target_compile_options(OlinCoin PRIVATE  -Wall -Wextra -pedantic) #-O2)