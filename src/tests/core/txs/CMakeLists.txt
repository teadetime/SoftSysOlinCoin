add_executable(test_base_tx
  test_base_tx.c ${SOURCES_ALL})
target_compile_options(test_base_tx PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_base_tx PUBLIC
  #${INCLUDE_ALL}
  # # Same as these
  ${INCLUDE_BLOCKS}
  ${INCLUDE_TX}
  ${INCLUDE_GLOBAL}
  )

  # # Link libraries for the mbed stuff if needed
# target_link_libraries(test_base_tx 
#   mbedtls
#   mbedcrypto
#   mbedx509)

# Look at the source variables and whatnot
message(STATUS "Txs Test: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Sources@ executable: ${SOURCES_ALL}")
message(STATUS "INCLUDE_BLOCKS @ executable: ${INCLUDE_BLOCKS}")
message(STATUS "INCLUDE_TX @ executable: ${INCLUDE_TX}")
message(STATUS "INCLUDE_GL @ executable: ${INCLUDE_GLOBAL}")
message(STATUS "INCLUDE_ALL @ executable: ${INCLUDE_ALL}")