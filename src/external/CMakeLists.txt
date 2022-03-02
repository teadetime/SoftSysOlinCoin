set(MBED_PROJECT_NAME "mbed_tls")
set(EXTERN_LIB_NAME "extern_lib")


include(FetchContent)

#option(ENABLE_PROGRAMS "Build mbed TLS programs." OFF)
set(ENABLE_TESTING OFF CACHE INTERNAL "<some description>")
set(ENABLE_PROGRAMS OFF CACHE INTERNAL "<some description>")
set(CMAKE_BUILD_TYPE Release CACHE INTERNAL "<some description>")
message(STATUS "external Cmake_BIN: ${CMAKE_BINARY_DIR}")
message(STATUS "external Cmake Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mbed_ouptut/)
message(STATUS "external after Cmake Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


FetchContent_Declare(MbedTLS
  GIT_REPOSITORY https://github.com/ARMmbed/mbedtls.git
  GIT_TAG v3.1.0
  # SOURCE_DIR    ${CMAKE_BINARY_DIR}/../external/src
  # BINARY_DIR    ${CMAKE_BINARY_DIR}/../external/bin
)

FetchContent_MakeAvailable(MbedTLS)
