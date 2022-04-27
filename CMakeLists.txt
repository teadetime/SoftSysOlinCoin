cmake_minimum_required(VERSION 3.13)

# set the project name
project(OlinCoin C CXX)
enable_testing()
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(INCLUDE_ALL "" CACHE INTERNAL "used internally")
set(INCLUDE_TX "" CACHE INTERNAL "used internally")
set(INCLUDE_BLOCK "" CACHE INTERNAL "used internally")
set(INCLUDE_GLOBAL "" CACHE INTERNAL "used internally")
set(INCLUDE_TEST "" CACHE INTERNAL "used internally")

set(UTHASH_IN_PROJ "src/includes/uthash/")
set(INCLUDE_ALL 
    ${INCLUDE_ALL}
    ${CMAKE_CURRENT_SOURCE_DIR}/${UTHASH_IN_PROJ} 
    CACHE INTERNAL "used internally")
set(UTHASH_DOWNLOAD_LINK "https://raw.githubusercontent.com/troydhanson/uthash/master/src/uthash.h")
find_file(UTHASH_LIB NAMES uthash.h PATHS ${CMAKE_CURRENT_SOURCE_DIR}/${UTHASH_IN_PROJ}/)
message(STATUS ${UTHASH_LIB})
if(NOT UTHASH_LIB)
  file(DOWNLOAD ${UTHASH_DOWNLOAD_LINK} ${CMAKE_CURRENT_SOURCE_DIR}/${UTHASH_IN_PROJ}/uthash.h)
  message(STATUS "Uthash not found")
endif()


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
