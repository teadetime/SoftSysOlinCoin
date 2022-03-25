set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/runtime/")
message(STATUS "Runtime Output set to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Building Runtime")

add_executable(shell
  ../core/globals/shell.c
  ${SOURCES_ALL})
target_compile_options(shell PRIVATE ${COMPILE_OPTIONS_STD})
message(STATUS "Core Globals Include: ${INCLUDE_ALL}")
message(STATUS "Core Globals Sources: ${SOURCES_ALL}")
target_include_directories(shell PUBLIC
  ${INCLUDE_ALL}
  # # Same as these
  # ${INCLUDE_BLOCKS}
  # ${INCLUDE_TX}
  # ${INCLUDE_GLOBAL}
  )

# Link libraries for the mbed stuff
target_link_libraries(shell  
  mbedtls
  mbedcrypto
  mbedx509)
