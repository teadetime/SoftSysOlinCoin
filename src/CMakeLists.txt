message(STATUS "root cmake Source: ${CMAKE_SOURCE_DIR}")
message(STATUS "root Cmake/Bin?: ${CMAKE_BINARY_DIR}")
message(STATUS "root Cmake Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_subdirectory(external)

add_subdirectory(includes)
add_subdirectory(core)
add_subdirectory(tests)


# add_executable(mbed ${CMAKE_SOURCE_DIR}/src/tests/core/blocks/mbed_test.c)
# target_link_libraries(mbed PUBLIC mbedtls
# mbedcrypto
# mbedx509)


