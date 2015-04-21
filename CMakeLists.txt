cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_broker)

find_package(BunsanCMake CONFIG REQUIRED)

bunsan_find_python3()

bunsan_add_protobuf_cxx_library(
    TARGET ${PROJECT_NAME}_pb
    PYTHON
    PROTOS
        bunsan/broker/common.proto
        bunsan/broker/client.proto
        bunsan/broker/worker.proto
        bunsan/broker/rabbitmq/common.proto
    INSTALL
)
bunsan_install_python_module_init(
    MODULE bunsan.broker
    INIT pysrc/bunsan/broker/__init__.py
)
bunsan_install_python_module_init(
    MODULE bunsan.broker.worker
    INIT pysrc/bunsan/broker/worker/__init__.py
)
#bunsan_add_library(${PROJECT_NAME}
#)
#bunsan_use_target(${PROJECT_NAME} ${PROJECT_NAME}_pb)
#bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_protobuf bunsan_protobuf)

#bunsan_add_executable(${PROJECT_NAME}_cli
#    src/bin/cli.cpp
#)
#bunsan_use_target(${PROJECT_NAME}_cli ${PROJECT_NAME})

bunsan_install_headers()
bunsan_install_targets(
    #${PROJECT_NAME}
    #${PROJECT_NAME}_cli
)
bunsan_install_project()

bunsan_include_tests()
