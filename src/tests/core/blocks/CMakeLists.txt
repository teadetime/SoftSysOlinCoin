add_executable(test_mbed_sign
  test_mbed_sign.c) # This doesn't need our sources currently ${SOURCES_ALL})
target_compile_options(test_mbed_sign PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_mbed_sign PUBLIC
  ${INCLUDE_ALL}
  # # Same as these
  # ${INCLUDE_BLOCKS}
  # ${INCLUDE_TX}
  # ${INCLUDE_GLOBAL}
  )

# Link libraries for the mbed stuff
target_link_libraries(test_mbed_sign  
  mbedtls
  mbedcrypto
  mbedx509)

add_executable(test_create_block
  test_create_block.c
  ${SOURCES_ALL})
target_compile_options(test_create_block PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_create_block PUBLIC
  ${INCLUDE_ALL}
  )
# Link libraries for the mbed stuff
target_link_libraries(test_create_block  
  mbedtls
  mbedcrypto
  mbedx509)


# Look at the source variables and whatnot
message(STATUS "Blocks Test: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Sources @ executable: ${TX_SOURCES}")
message(STATUS "INCLUDE_BLOCKS @ executable: ${INCLUDE_BLOCKS}")
message(STATUS "INCLUDE_TX @ executable: ${INCLUDE_TX}")
message(STATUS "INCLUDE_GL @ executable: ${INCLUDE_GLOBAL}")
message(STATUS "INCLUDE_ALL @ executable: ${INCLUDE_ALL}")