bunsan_tests_project_header()

bunsan_tests_use_parent_project()
if(UNIX)
    bunsan_tests_use_package(Boost coroutine context thread)
endif()
bunsan_tests_use_bunsan_package(bunsan_test bunsan_test)

bunsan_tests_project_aux_add_tests()

add_subdirectory(generators)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/stream_enum.cpp
    COMMAND test_generator_stream_enum ${CMAKE_CURRENT_BINARY_DIR}/stream_enum.cpp
    DEPENDS test_generator_stream_enum
)
bunsan_tests_project_add_test(stream_enum
    ${CMAKE_CURRENT_BINARY_DIR}/stream_enum.cpp
)

bunsan_tests_project_footer()
