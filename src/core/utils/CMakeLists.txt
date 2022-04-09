message(STATUS "In Core Utils: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(crypto STATIC crypto.c)
target_include_directories(crypto PUBLIC ${INCLUDE_ALL})
target_link_libraries(crypto
  mbedtls)
