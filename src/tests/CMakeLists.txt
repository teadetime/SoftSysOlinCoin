# message(STATUS "root cmake Source: ${CMAKE_SOURCE_DIR}")
# message(STATUS "root Cmake/Bin?: ${CMAKE_BINARY_DIR}")
# message(STATUS "root Cmake Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/")
message(STATUS "Setting Test Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_subdirectory(core)



