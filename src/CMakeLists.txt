message(STATUS "root cmake Source: ${CMAKE_SOURCE_DIR}")
message(STATUS "root Cmake/Bin?: ${CMAKE_BINARY_DIR}")
message(STATUS "root Cmake Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
# Build mbed tls first!
add_subdirectory(external)
# Then Let's add the include files
add_subdirectory(includes)
# Then Build sources core
add_subdirectory(core)
# Then Build tests
IF(BUILD_TESTS)
  message(STATUS "Building Tests!")
  add_subdirectory(tests)
ENDIF()
IF(BUILD_RUNTIME)
  message(STATUS "Building Runtime!")
  add_subdirectory(runtime)
ENDIF()

# add_executable(mbed ${CMAKE_SOURCE_DIR}/src/tests/core/blocks/mbed_test.c)
# target_link_libraries(mbed PUBLIC mbedtls
# mbedcrypto
# mbedx509)


