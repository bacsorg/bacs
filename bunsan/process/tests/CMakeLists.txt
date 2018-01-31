bunsan_tests_project_header()

bunsan_tests_use_parent_project()
bunsan_tests_use_bunsan_package(bunsan_test bunsan_test)

bunsan_tests_project_aux_add_tests()
bunsan_tests_project_add_test(
    ${platform_tests}_process
    ${platform_tests}/process.cpp
)

bunsan_tests_project_footer()
