project(bunsan_bacs_archive)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_boost(system filesystem serialization unit_test_framework program_options)
bunsan_use_bunsan(common process utility pm)

bunsan_add_shared_library(${PROJECT_NAME}
    src/lib/repository.cpp
)

bunsan_add_executable(${PROJECT_NAME}_cli src/cli.cpp)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME} ${PROJECT_NAME}_cli)

bunsan_include_tests()
