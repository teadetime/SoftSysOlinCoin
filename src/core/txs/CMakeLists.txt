message(STATUS "In Core Txs: ${CMAKE_CURRENT_SOURCE_DIR}")
set(TX_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/base_tx.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/sign_tx.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/wallet.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/wallet_pool.c 
  # ADD TX SOURCES HERE
  CACHE INTERNAL "")
set(SOURCES_ALL 
  ${SOURCES_ALL}
  ${TX_SOURCES}
  # ADD SOURCES HERE
  CACHE INTERNAL ""
  )


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