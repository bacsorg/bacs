cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bacs_external)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

bunsan_add_protobuf_cxx_library(
    TARGET ${PROJECT_NAME}_pb
    PROTOS
        bacs/external/contest.proto
        bacs/external/external.proto
        bacs/external/language.proto
        bacs/external/result.proto
        bacs/external/submit.proto
    GRPC
    LIBRARIES ${bunsan_common_LIBRARIES}
    INCLUDE_DIRECTORIES ${bunsan_common_INCLUDE_DIRS}
    INSTALL
)

bunsan_add_library(${PROJECT_NAME}
    src/lib/judge.cpp
)
bunsan_use_target(${PROJECT_NAME} ${PROJECT_NAME}_pb)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_curl bunsan_curl)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_protobuf bunsan_protobuf)

bunsan_add_executable(${PROJECT_NAME}_cli
    src/bin/cli.cpp
)
bunsan_use_target(${PROJECT_NAME}_cli ${PROJECT_NAME})

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME} ${PROJECT_NAME}_cli)
bunsan_install_project()

bunsan_include_tests()
