add_library(fixtures STATIC
    fixtures_tx.c
    fixtures_wallet.c
    fixtures_block.c
    fixtures_global.c)
target_include_directories(fixtures
  PUBLIC ${INCLUDE_ALL}
  PUBLIC ${INCLUDE_FIXTURES})
target_link_libraries(fixtures
  core_tx
  core_block
  wallet_pool
  blockchain
  mempool
  utxo_to_tx
  crypto
  cmocka-static)
