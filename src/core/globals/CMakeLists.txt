message(STATUS "In Core Globals: ${CMAKE_CURRENT_SOURCE_DIR}")
set(GLOBAL_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/blockchain.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/constants.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/mempool.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/utxo_pool.c   
  ${CMAKE_CURRENT_SOURCE_DIR}/list.c 
  # ADD TX SOURCES HERE
  CACHE INTERNAL "")
set(SOURCES_ALL 
  ${SOURCES_ALL}
  ${GLOBAL_SOURCES}
  # ADD SOURCES HERE
  CACHE INTERNAL ""
  )

add_library(constants STATIC constants.c)
target_include_directories(constants PUBLIC ${INCLUDE_ALL})
target_link_libraries(constants
  mbedtls)

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