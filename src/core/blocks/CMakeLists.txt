message(STATUS "In Core Blocks: ${CMAKE_CURRENT_SOURCE_DIR}")
set(BLOCK_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/base_block.c
  ${CMAKE_CURRENT_SOURCE_DIR}/create_block.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/validate_block.c 
  # ADD TX SOURCES HERE
  CACHE INTERNAL "")
set(SOURCES_ALL 
  ${SOURCES_ALL}
  ${BLOCK_SOURCES}
  # ADD SOURCES HERE
  CACHE INTERNAL ""
  )

add_library(core_block STATIC base_block.c)
target_include_directories(core_block PUBLIC ${INCLUDE_ALL})
target_link_libraries(core_block
  core_tx)

add_library(core_mine STATIC create_block.c validate_block.c)
target_include_directories(core_mine PUBLIC ${INCLUDE_ALL})
target_link_libraries(core_mine
  mempool
  blockchain
  utxopool
  wallet_pool)