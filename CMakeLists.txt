project(bunsan_bacs_statement_provider)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_boost(system filesystem serialization)
bunsan_use_bunsan(common process utility pm bacs_problem bacs_problems)
bunsan_use(cryptopp)
bunsan_use_protobuf()

bunsan_protobuf_generate_cpp(SOURCES proto_srcs HEADERS proto_hdrs INSTALL include PROTOS
    bacs/statement_provider/request.proto
)

bunsan_add_shared_library(${PROJECT_NAME}
    ${proto_srcs}
)
bunsan_use(${PROJECT_NAME})

bunsan_use_bunsan(network web)
bunsan_use(cppcms booster)
bunsan_add_executable(${PROJECT_NAME}_web
    src/bin/web.cpp
    src/web/server.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME} ${PROJECT_NAME}_web)

bunsan_include_tests()
