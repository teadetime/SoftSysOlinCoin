# Blockchain Test
add_executable(test_blockchain test_blockchain.c)
target_compile_options(test_blockchain PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_blockchain PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_blockchain
  blockchain)

# Mempool Test
add_executable(test_mempool test_mempool.c)
target_compile_options(test_mempool PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_mempool PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_mempool 
  mempool)

# Test UTXO
add_executable(test_utxo_pool  test_utxo_pool.c)
target_compile_options(test_utxo_pool PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_utxo_pool PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_utxo_pool 
  utxopool)

# Test Constants 
add_executable(test_constants test_constants.c)
target_compile_options(test_constants PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_constants PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_constants 
  constants)

#Test List
add_executable(test_list test_list.c)
target_compile_options(test_list PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_list PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_list
  lists)