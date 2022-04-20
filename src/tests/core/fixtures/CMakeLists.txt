add_library(fixtures STATIC fixtures_tx.c)
target_include_directories(fixtures
  PUBLIC ${INCLUDE_ALL}
  PUBLIC ${INCLUDE_FIXTURES})
target_link_libraries(fixtures
  core_tx
  crypto)

message(STATUS "In test core fixtures")
message(STATUS "${INCLUDE_ALL}")
message(STATUS "${INCLUDE_FIXTURES}")
