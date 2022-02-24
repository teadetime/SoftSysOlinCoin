cmake_minimum_required(VERSION 3.10)

# set the project name
project(OlinCoin C)

# ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# add the executable
add_executable(OlinCoin
    ${CMAKE_SOURCE_DIR}/tests/core/txs/test_base_tx.c 
    ${CMAKE_SOURCE_DIR}/core/txs/base_tx.c)

target_include_directories(OlinCoin PUBLIC
    "${PROJECT_BINARY_DIR}/includes/txs"
    "${PROJECT_BINARY_DIR}/includes/globals"
    )

target_compile_options(OlinCoin PRIVATE  -Wall -Wextra -O2)