project(bunsan_protobuf)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_bunsan(common)

bunsan_use_protobuf()

bunsan_add_shared_library(${PROJECT_NAME}
    src/serialize.cpp
    src/parse.cpp
    src/merge.cpp
    src/comparator.cpp
    src/string.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})

if(ENABLE_TESTS)
    bunsan_protobuf_generate_cpp(SOURCES proto_srcs
                                 HEADERS proto_hdrs
                                 INSTALL include
                                 PROTOS
        bunsan/protobuf/detail/testing/message.proto
    )
    set(test_pb_library ${PROJECT_NAME}_test_pb)
    bunsan_add_shared_library(${test_pb_library}
        ${proto_srcs}
    )
endif()

bunsan_include_tests()
