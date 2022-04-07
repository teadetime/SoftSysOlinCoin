message(STATUS "In Core Globals: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(mempool STATIC mempool.c)
target_include_directories(mempool PUBLIC ${INCLUDE_ALL})
target_link_libraries(mempool
  core_tx)

add_library(utxopool STATIC utxo_pool.c)
target_include_directories(utxopool PUBLIC ${INCLUDE_ALL})
target_link_libraries(utxopool
  core_tx)

add_library(lists STATIC list.c)
target_include_directories(lists PUBLIC ${INCLUDE_ALL})

add_library(blockchain STATIC blockchain.c)
target_include_directories(blockchain PUBLIC ${INCLUDE_ALL})
target_link_libraries(blockchain
  core_block)

add_library(constants STATIC constants.c)
target_include_directories(constants PUBLIC ${INCLUDE_ALL})
target_link_libraries(constants
  mempool
  blockchain
  utxopool
  mbedtls)

add_library(utxo_to_tx STATIC utxo_to_tx.c)
target_include_directories(utxo_to_tx PUBLIC ${INCLUDE_ALL})
target_link_libraries(utxo_to_tx
  utxopool)

add_library(double_spend STATIC double_spend_set.c)
target_include_directories(double_spend PUBLIC ${INCLUDE_ALL})
target_link_libraries(double_spend
  utxopool)
