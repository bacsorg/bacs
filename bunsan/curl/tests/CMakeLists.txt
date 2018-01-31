bunsan_tests_project_header()

bunsan_tests_use_parent_project()

bunsan_find_python3()
macro(bunsan_curl_tests_add_server_test target)
    bunsan_tests_project_add_executable(test_${target} ${ARGN})
    set(test_executable ${CMAKE_CURRENT_BINARY_DIR}/test_${target})
    bunsan_tests_project_add_custom_test(${target}
        ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/server.py
        ${test_executable}
    )
endmacro()

bunsan_tests_project_add_test(callback callback.cpp)
bunsan_tests_project_add_test(easy easy.cpp)
bunsan_tests_project_add_test(http http.cpp)
bunsan_tests_project_add_test(option_set option_set.cpp)
bunsan_tests_project_add_test(slist slist.cpp)
bunsan_tests_project_add_test(traits traits.cpp)
bunsan_tests_project_add_test(wrapper wrapper.cpp)

bunsan_curl_tests_add_server_test(easy_info easy_info.cpp)
bunsan_curl_tests_add_server_test(easy_options easy_options.cpp)
bunsan_curl_tests_add_server_test(http_client http_client.cpp)

bunsan_tests_project_footer()
