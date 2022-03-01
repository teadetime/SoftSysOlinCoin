# add_executable(test_global
#   some global test.c) # This doesn't need our sources currently ${SOURCES_ALL})
# target_compile_options(test_global PRIVATE ${COMPILE_OPTIONS_STD})
# target_include_directories(test_global PUBLIC
#   ${INCLUDE_ALL}
#   # # Same as these
#   # ${INCLUDE_BLOCKS}
#   # ${INCLUDE_TX}
#   # ${INCLUDE_GLOBAL}
#   )

# # Link libraries for the mbed stuff if needed
# target_link_libraries(test_global  
#   mbedtls
#   mbedcrypto
#   mbedx509)
# # Look at the source variables and whatnot
# message(STATUS "Globals Test: ${CMAKE_CURRENT_SOURCE_DIR}")
# message(STATUS "Sources @ executable: ${TX_SOURCES}")
# message(STATUS "INCLUDE_BLOCKS @ executable: ${INCLUDE_BLOCKS}")
# message(STATUS "INCLUDE_TX @ executable: ${INCLUDE_TX}")
# message(STATUS "INCLUDE_GL @ executable: ${INCLUDE_GLOBAL}")
# message(STATUS "INCLUDE_ALL @ executable: ${INCLUDE_ALL}")