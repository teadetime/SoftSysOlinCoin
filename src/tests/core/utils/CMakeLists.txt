# Test Constants 
add_executable(test_crypto test_crypto.c)
target_compile_options(test_crypto PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(test_crypto PUBLIC ${INCLUDE_ALL})
target_link_libraries(test_crypto 
  constants)

add_test(test_crypto ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_crypto)
