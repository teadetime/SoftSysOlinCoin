set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/runtime/")
message(STATUS "Runtime Output set to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Building Runtime")
# Add inlcudes for the runtime headers
add_subdirectory(includes)

add_library(shell shell.c)
target_compile_options(shell PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(shell PUBLIC
  ${INCLUDE_ALL}
  )
target_link_libraries(shell  
  core_mine
  wallet
  handle_block
  init_globals)

add_library(server server.c)
target_compile_options(server PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(server PUBLIC
  ${INCLUDE_ALL}
  )
target_link_libraries(server
  core_block
  rt
)

add_library(client client.c)
target_compile_options(client PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(client PUBLIC
  ${INCLUDE_ALL}
  )
target_link_libraries(client
  core_block
  rt
)
add_executable(runtime runtime.c)
target_compile_options(runtime PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(runtime PUBLIC ${INCLUDE_ALL})
target_link_libraries(runtime
  queue
  handle_block
  Threads::Threads
  init_globals
  validate_tx
  handle_tx
  shell
  server
  client)

add_executable(q_server queue_server.c)
target_compile_options(q_server PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(q_server PUBLIC ${INCLUDE_ALL})
target_link_libraries(q_server rt)

add_executable(q_client queue_client.c)
target_compile_options(q_client PRIVATE ${COMPILE_OPTIONS_STD})
target_include_directories(q_client PUBLIC ${INCLUDE_ALL})
target_link_libraries(q_client rt)