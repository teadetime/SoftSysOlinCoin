add_executable(waiter_test waiter_test_wrap.c chef.c)
target_link_libraries(waiter_test cmocka-static)

add_test(waiter_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/waiter_test)
set_target_properties(waiter_test
        PROPERTIES
        LINK_FLAGS  "-Wl,--wrap=chef_cook"
)
