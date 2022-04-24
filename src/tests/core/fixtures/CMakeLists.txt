add_library(fixtures STATIC fixtures_tx.c fixtures_wallet.c)
target_include_directories(fixtures
  PUBLIC ${INCLUDE_ALL}
  PUBLIC ${INCLUDE_FIXTURES})
target_link_libraries(fixtures
  core_tx
  wallet_pool
  crypto
  cmocka-static)
