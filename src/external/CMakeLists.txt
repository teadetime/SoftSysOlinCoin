set(MBED_PROJECT_NAME "mbed_tls")
set(EXTERN_LIB_NAME "extern_lib")


include(FetchContent)

set(LEVELDB_BUILD_TESTS OFF CACHE INTERNAL "<some description>")
set(LEVELDB_BUILD_BENCHMARKS OFF CACHE INTERNAL "<some description>")
set(LEVELDB_INSTALL OFF CACHE INTERNAL "<some description>")

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
  GIT_SHALLOW    1
)
FetchContent_MakeAvailable(MbedTLS)

FetchContent_Declare(levelDB
  GIT_REPOSITORY https://github.com/google/leveldb.git
  GIT_TAG 1.23
  GIT_SHALLOW    1
)
FetchContent_MakeAvailable(levelDB)