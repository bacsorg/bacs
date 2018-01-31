cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_protobuf)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

bunsan_add_library(${PROJECT_NAME}
    src/base_io.cpp
    src/base_parser.cpp
    src/base_serializer.cpp
    src/binary_parser.cpp
    src/binary_serializer.cpp
    src/comparator.cpp
    src/text_parser.cpp
    src/text_serializer.cpp
)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_common bunsan_common)
bunsan_use_package(${PROJECT_NAME} Protobuf)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_project()

if(ENABLE_TESTS)
    bunsan_add_protobuf_cxx_library(
        TARGET ${PROJECT_NAME}_test_pb
        STATIC
        PROTOS
            bunsan/protobuf/test/message.proto
    )
endif()

bunsan_include_tests()
