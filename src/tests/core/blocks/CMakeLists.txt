# Test create block
add_executable(test_create_block test_create_block.c)
target_compile_options(test_create_block PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_create_block PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_create_block  
  core_mine)

# Test validate block
add_executable(test_validate_block test_validate_block.c)
target_compile_options(test_validate_block PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_validate_block PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_validate_block
  core_mine)

# Look at the source variables and whatnot
message(STATUS "Blocks Test: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Sources @ executable: ${TX_SOURCES}")
message(STATUS "INCLUDE_BLOCKS @ executable: ${INCLUDE_BLOCKS}")
message(STATUS "INCLUDE_TX @ executable: ${INCLUDE_TX}")
message(STATUS "INCLUDE_GL @ executable: ${INCLUDE_GLOBAL}")
message(STATUS "INCLUDE_ALL @ executable: ${INCLUDE_ALL}")
