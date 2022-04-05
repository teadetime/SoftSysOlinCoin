set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/runtime/")
message(STATUS "Runtime Output set to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Building Runtime")

add_executable(shell shell.c)
target_compile_options(shell PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(shell PUBLIC
  ${INCLUDE_ALL}
  "${CMAKE_CURRENT_SOURCE_DIR}/includes"
  )
target_link_libraries(shell  
  core_mine)
