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

# Test handle block
add_executable(test_handle_block test_handle_block.c)
target_compile_options(test_handle_block PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_handle_block PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_handle_block
  handle_block)

# Test ser block
add_executable(test_ser_block test_ser_block.c)
target_compile_options(test_ser_block PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_ser_block
    PUBLIC ${INCLUDE_ALL}
    PUBLIC ${INCLUDE_FIXTURES})
target_link_libraries(test_ser_block
  core_mine
  fixtures)

add_test(test_create_block ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_create_block)
add_test(test_validate_block ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_validate_block)
add_test(test_handle_block ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_handle_block)
add_test(test_ser_block ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_ser_block)
