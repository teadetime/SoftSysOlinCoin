message(STATUS "In Core Blocks: ${CMAKE_CURRENT_SOURCE_DIR}")
add_library(core_block STATIC base_block.c ser_block.c)
target_include_directories(core_block PUBLIC ${INCLUDE_ALL})
target_link_libraries(core_block
  core_tx)

add_library(core_mine STATIC create_block.c validate_block.c)
target_include_directories(core_mine PUBLIC ${INCLUDE_ALL})
target_link_libraries(core_mine
  mempool
  blockchain
  utxopool
  wallet_pool
  validate_tx)

add_library(handle_block STATIC handle_block.c)
target_include_directories(handle_block PUBLIC ${INCLUDE_ALL})
target_link_libraries(handle_block
  core_mine)
