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

add_test(test_create_block ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_create_block)
add_test(test_validate_block ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_validate_block)
