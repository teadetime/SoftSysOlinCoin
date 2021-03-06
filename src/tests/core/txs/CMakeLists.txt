# Test base Tx
add_executable(test_base_tx test_base_tx.c)
target_compile_options(test_base_tx PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_base_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_base_tx  
  core_tx)

add_executable(test_ser_tx test_ser_tx.c)
target_compile_options(test_ser_tx PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_ser_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_ser_tx  
  core_tx)

add_executable(test_wallet_pool test_wallet_pool.c)
target_compile_options(test_wallet_pool PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_wallet_pool PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_wallet_pool  
  wallet_pool)

add_executable(test_ser_wallet test_ser_wallet.c)
target_compile_options(test_ser_wallet PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_ser_wallet PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_ser_wallet  
  wallet_pool)

add_executable(test_wallet test_wallet.c)
target_compile_options(test_wallet PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_wallet PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_wallet  
  wallet)

add_executable(test_validate_tx test_validate_tx.c)
target_compile_options(test_validate_tx PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_validate_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_validate_tx 
  validate_tx)

add_executable(test_handle_tx test_handle_tx.c)
target_compile_options(test_handle_tx PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_handle_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_handle_tx 
  handle_tx)

add_test(test_base_tx ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_base_tx)
add_test(test_ser_tx ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_ser_tx)
add_test(test_wallet ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_wallet)
add_test(test_wallet_pool ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_wallet_pool)
add_test(test_ser_wallet ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_ser_wallet)
add_test(test_validate_tx ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_validate_tx)
add_test(test_handle_tx ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_handle_tx)
add_test(test_ser_tx ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_ser_tx)
