message(STATUS "In Core Txs: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(core_tx STATIC base_tx.c ser_tx.c)
target_include_directories(core_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(core_tx
  crypto)

add_library(wallet_pool STATIC wallet_pool.c)
target_include_directories(wallet_pool PUBLIC ${INCLUDE_ALL})
target_link_libraries(wallet_pool
  core_tx
  leveldb)

add_library(wallet STATIC wallet.c)
target_include_directories(wallet PUBLIC ${INCLUDE_ALL})
target_link_libraries(wallet
  wallet_pool)

add_library(validate_tx STATIC validate_tx.c)
target_include_directories(validate_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(validate_tx
  core_tx
  utxo_to_tx
  double_spend)

add_library(handle_tx STATIC handle_tx.c)
target_include_directories(handle_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(handle_tx
  core_tx
  utxo_to_tx
  mempool)