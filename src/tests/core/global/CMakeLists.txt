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
  utxopool
  init_db)

# Test List
add_executable(test_list test_list.c)
target_compile_options(test_list PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_list PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_list
  lists)

# Test UTXO To Tx
add_executable(test_utxo_to_tx test_utxo_to_tx.c)
target_compile_options(test_utxo_to_tx PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_utxo_to_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_utxo_to_tx
  utxo_to_tx
  core_tx)

add_test(test_list ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_list)
add_test(test_utxo_pool ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_utxo_pool)
add_test(test_mempool ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_mempool)
add_test(test_blockchain ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_blockchain)
add_test(test_utxo_to_tx ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_utxo_to_tx)

add_subdirectory(chef_wrap)