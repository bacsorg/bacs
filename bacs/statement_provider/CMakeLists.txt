cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bacs_statement_provider)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

if(BunsanWebBuiltin)
    BunsanWebLoad()
else()
    find_package(BunsanWeb CONFIG REQUIRED)
endif()

bunsan_find_bunsan_package(bacs_problems bacs_problems)

bunsan_add_protobuf_cxx_library(
    TARGET ${PROJECT_NAME}
    PROTOS
        bacs/statement_provider/request.proto
    LIBRARIES ${bacs_problems_LIBRARIES}
    INCLUDE_DIRECTORIES ${bacs_problems_INCLUDE_DIRS}
    INSTALL
)

bunsan_add_executable(${PROJECT_NAME}_web
    src/bin/web.cpp
    src/web/server.cpp
)
bunsan_use_target(${PROJECT_NAME}_web ${PROJECT_NAME})
bunsan_use_bunsan_package(${PROJECT_NAME}_web bunsan_pm bunsan_pm)
bunsan_use_bunsan_package(${PROJECT_NAME}_web bunsan_web bunsan_web)
bunsan_use_package(${PROJECT_NAME}_web Botan)
bunsan_use_package(${PROJECT_NAME}_web CppCMS)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME}_web)
bunsan_install_project()

bunsan_include_tests()
