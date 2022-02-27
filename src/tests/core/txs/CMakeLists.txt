
message(STATUS "Txs Test: ${CMAKE_SOURCE_DIR}")
# add_executable(test_base_tx test_base_tx.c)

# target_link_libraries(test_base_tx PUBLIC base_tx_lib)




  message(STATUS "Sources @ executable: ${TX_SOURCES}")
add_executable(nathan
  test_base_tx.c ${SOURCES_ALL})

message(STATUS "INCLUDE_BLOCKS @ executable: ${INCLUDE_BLOCKS}")
message(STATUS "INCLUDE_TX @ executable: ${INCLUDE_TX}")
message(STATUS "INCLUDE_GL @ executable: ${INCLUDE_GLOBAL}")

message(STATUS "INCLUDE_ALL @ executable: ${INCLUDE_ALL}")


target_include_directories(nathan PUBLIC
  ${INCLUDE_ALL}
  # Same as these
#   ${INCLUDE_BLOCKS}
# ${INCLUDE_TX}
# ${INCLUDE_GLOBAL}
  )