cmake_minimum_required(VERSION 3.13)

# set the project name
project(OlinCoin C)

# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(INCLUDE_ALL "" CACHE INTERNAL "used internally")
set(SOURCES_ALL "" CACHE INTERNAL "used internally")


set(INCLUDE_TX "" CACHE INTERNAL "used internally")
set(INCLUDE_BLOCK "" CACHE INTERNAL "used internally")
set(INCLUDE_GLOBAL "" CACHE INTERNAL "used internally")
set(INCLUDE_TEST "" CACHE INTERNAL "used internally")
set(SOURCES_TX "" CACHE INTERNAL "used internally")
set(SOURCES_BLOCK "" CACHE INTERNAL "used internally")
set(SOURCES_GLOBAL "" CACHE INTERNAL "used internally")

set(COMPILE_OPTIONS_STD -Wall -Wextra -pedantic -O2 CACHE INTERNAL "used internally")
set(BUILD_TESTS ON)
set(BUILD_RUNTIME ON)  
add_subdirectory(src)

message(STATUS "Top Level")
message(STATUS "CLEAN: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests")
add_custom_target(_clean
  #COMMAND "$(CMAKE)" --build "${CMAKE_BINARY_DIR}" clean
  COMMAND cd ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} &&  rm -rf tests/* 
  COMMAND cd ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} &&  rm -rf runtime/* 
  COMMAND cd "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
  COMMAND rm -r "CMakeFiles"
  COMMAND rm -r "cmake_install.cmake"
  COMMAND rm -r "CMakeCache.txt"
)
