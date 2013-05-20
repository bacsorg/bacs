project(bunsan_bacs_problem)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_bunsan(common pm bacs_single_api bacs_single_problem)
bunsan_use_boost(system filesystem serialization unit_test_framework thread)
bunsan_use_protobuf()

bunsan_add_shared_library(${PROJECT_NAME}
    src/common.cpp
    src/importer.cpp

    src/importers/single.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})

bunsan_include_tests()
