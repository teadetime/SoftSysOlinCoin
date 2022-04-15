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
  # SOURCE_DIR    ${CMAKE_BINARY_DIR}/../external/src
  # BINARY_DIR    ${CMAKE_BINARY_DIR}/../external/bin
)
FetchContent_MakeAvailable(MbedTLS)

FetchContent_Declare(check
  GIT_REPOSITORY https://github.com/libcheck/check.git
  GIT_TAG 0.15.2
  # SOURCE_DIR    ${CMAKE_BINARY_DIR}/../external/src
  # BINARY_DIR    ${CMAKE_BINARY_DIR}/../external/bin
)
FetchContent_MakeAvailable(check)

FetchContent_Declare(check
  GIT_REPOSITORY https://github.com/libcheck/check.git
  GIT_TAG 0.15.2
  # SOURCE_DIR    ${CMAKE_BINARY_DIR}/../external/src
  # BINARY_DIR    ${CMAKE_BINARY_DIR}/../external/bin
)
FetchContent_MakeAvailable(check)

# Also specify GIT_SHALLOW to avoid cloning branch we don't care about
FetchContent_Declare(
  cmocka
  GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git
  GIT_TAG        cmocka-1.1.5
  GIT_SHALLOW    1
)
FetchContent_MakeAvailable(check)

# We want to link to cmocka-static, so we need to set this option before calling the FetchContent_MakeAvailable
# We also don't care about example and tests
set(WITH_STATIC_LIB ON CACHE BOOL "CMocka: Build with a static library" FORCE)
set(WITH_CMOCKERY_SUPPORT OFF CACHE BOOL "CMocka: Install a cmockery header" FORCE)
set(WITH_EXAMPLES OFF CACHE BOOL "CMocka: Build examples" FORCE)
set(UNIT_TESTING OFF CACHE BOOL "CMocka: Build with unit testing" FORCE)
set(PICKY_DEVELOPER OFF CACHE BOOL "CMocka: Build with picky developer flags" FORCE)

# Download cmocka, and execute its cmakelists.txt
FetchContent_MakeAvailable(cmocka) 