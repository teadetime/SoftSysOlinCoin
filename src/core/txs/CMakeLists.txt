message(STATUS "In Core Txs: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(core_tx STATIC sign_tx.c base_tx.c)
target_include_directories(core_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(core_tx
  constants)

add_library(wallet_pool STATIC wallet_pool.c)
target_include_directories(wallet_pool PUBLIC ${INCLUDE_ALL})
target_link_libraries(wallet_pool
  core_tx)

add_library(wallet STATIC wallet.c)
target_include_directories(wallet PUBLIC ${INCLUDE_ALL})
target_link_libraries(wallet
  wallet_pool)

add_library(validate_tx STATIC validate_tx.c)
target_include_directories(validate_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(validate_tx
  core_block
  utxo_to_tx
  double_spend)