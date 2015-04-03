cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_bacs_statement_provider)

find_package(BunsanCMake CONFIG REQUIRED)

bunsan_find_bunsan_package(bunsan_bacs_problems bunsan_bacs_problems)

bunsan_add_protobuf_cxx_library(
    TARGET ${PROJECT_NAME}
    PROTOS
        bacs/statement_provider/request.proto
    LIBRARIES ${bunsan_bacs_problems_LIBRARIES} cryptopp
    INCLUDE_DIRECTORIES ${bunsan_bacs_problems_INCLUDE_DIRS}
    INSTALL
)

bunsan_add_executable(${PROJECT_NAME}_web
    src/bin/web.cpp
    src/web/server.cpp
)
bunsan_use_target(${PROJECT_NAME}_web ${PROJECT_NAME})
bunsan_use_bunsan_package(${PROJECT_NAME}_web bunsan_network bunsan_network)
bunsan_use_bunsan_package(${PROJECT_NAME}_web bunsan_pm bunsan_pm)
bunsan_use_bunsan_package(${PROJECT_NAME}_web bunsan_web bunsan_web)
target_link_libraries(${PROJECT_NAME}_web PUBLIC cppcms booster)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME}_web)
bunsan_install_project()

bunsan_include_tests()
