cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_curl)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

bunsan_add_library(${PROJECT_NAME}
    src/easy.cpp
    src/easy_escape.cpp
    src/error_category.cpp
    src/global.cpp
    src/http_version.cpp

    src/http/client.cpp
    src/http/header.cpp
    src/http/header_parser.cpp
    src/http/header_set.cpp
    src/http/response_head.cpp
    src/http/status.cpp

    src/options/option_set.cpp

    src/detail/curl_delete.cpp
    src/detail/slist.cpp
    src/detail/string_list.cpp
)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_common bunsan_common)
bunsan_use_package(${PROJECT_NAME} CURL)
bunsan_use_boost(${PROJECT_NAME} regex)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_project()

bunsan_include_tests()
