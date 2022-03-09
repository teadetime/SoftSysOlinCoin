add_executable(test_blockchain
  test_blockchain.c
  ${SOURCES_ALL})
target_compile_options(test_blockchain PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_blockchain PUBLIC
  ${INCLUDE_ALL}
  )
# Link libraries for the mbed stuff
target_link_libraries(test_blockchain  
  mbedtls
  mbedcrypto
  mbedx509)

add_executable(test_mempool
  test_mempool.c
  ${SOURCES_ALL})
target_compile_options(test_mempool PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_mempool PUBLIC
  ${INCLUDE_ALL}
  )
# Link libraries for the mbed stuff
target_link_libraries(test_mempool  
  mbedtls
  mbedcrypto
  mbedx509)

add_executable(test_utxo_pool
  test_utxo_pool.c
  ${SOURCES_ALL})
target_compile_options(test_utxo_pool PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_utxo_pool PUBLIC
  ${INCLUDE_ALL}
  )
# Link libraries for the mbed stuff
target_link_libraries(test_utxo_pool  
  mbedtls
  mbedcrypto
  mbedx509)

add_executable(test_constants
  test_constants.c
  ${SOURCES_ALL})
target_compile_options(test_constants PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_constants PUBLIC
  ${INCLUDE_ALL}
  )
target_link_libraries(test_constants 
  mbedtls
  mbedcrypto
  mbedx509)

add_executable(test_list
  test_list.c
  ${SOURCES_ALL})
target_compile_options(test_list PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_list PUBLIC
  ${INCLUDE_ALL}
  )
target_link_libraries(test_list 
  mbedtls
  mbedcrypto
  mbedx509)

# # Link libraries for the mbed stuff if needed
# target_link_libraries(test_global  
#   mbedtls
#   mbedcrypto
#   mbedx509)
# # Look at the source variables and whatnot
# message(STATUS "Globals Test: ${CMAKE_CURRENT_SOURCE_DIR}")
# message(STATUS "Sources @ executable: ${TX_SOURCES}")
# message(STATUS "INCLUDE_BLOCKS @ executable: ${INCLUDE_BLOCKS}")
# message(STATUS "INCLUDE_TX @ executable: ${INCLUDE_TX}")
# message(STATUS "INCLUDE_GL @ executable: ${INCLUDE_GLOBAL}")
# message(STATUS "INCLUDE_ALL @ executable: ${INCLUDE_ALL}")
