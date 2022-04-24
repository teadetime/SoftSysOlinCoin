add_executable(test_crypto test_crypto.c)
target_compile_options(test_crypto PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_crypto PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_crypto 
  crypto
  cmocka-static)

add_executable(test_ser_key test_ser_key.c)
target_compile_options(test_ser_key PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_ser_key
    PUBLIC ${INCLUDE_ALL}
    PUBLIC ${INCLUDE_FIXTURES})
target_link_libraries(test_ser_key 
  crypto
  fixtures)

add_test(test_crypto ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_crypto)
add_test(test_ser_key ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_ser_key)
