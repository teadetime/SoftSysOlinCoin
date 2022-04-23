message(STATUS "In Core Utils: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(crypto STATIC crypto.c ser_key.c)
target_include_directories(crypto PUBLIC ${INCLUDE_ALL})
target_link_libraries(crypto
  mbedtls)
  
add_library(init_db STATIC init_db.c)
target_include_directories(init_db PUBLIC ${INCLUDE_ALL})
target_link_libraries(init_db
  leveldb)

add_library(queue STATIC queue.c)
target_include_directories(queue PUBLIC ${INCLUDE_ALL})
target_link_libraries(queue
  core_tx
  core_block)