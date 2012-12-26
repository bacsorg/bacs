project(bunsan_bacs_archive)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanWeb)

bunsan_use_boost(system filesystem serialization unit_test_framework program_options)
bunsan_use_bunsan(common process utility pm)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
bunsan_use(${PROTOBUF_LIBRARIES})

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/bacs)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/archive)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/pb)

# Compile proto files
set(PROTOBUF_IMPORT_DIRS proto)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(proto_srcs proto_hdrs
    proto/archive.proto
)

bunsan_add_shared_library(${PROJECT_NAME}
    ${proto_srcs}
    src/lib/repository.cpp
    src/lib/repository_impl.cpp
    src/lib/problem.cpp
    src/lib/config.cpp
)
bunsan_use(${PROJECT_NAME})

bunsan_add_executable(${PROJECT_NAME}_cli src/bin/cli.cpp)

bunsan_use_bunsan(network web)
bunsan_use(cppcms booster)
bunsan_add_executable(${PROJECT_NAME}_web
    src/web/repository.cpp
    src/web/content/form/base.cpp
    src/web/content/form/archiver_config.cpp
    src/web/content/form/list_query.cpp
    src/web/content/form/optional_list_query.cpp
    src/web/content/form/insert.cpp
    src/web/content/form/extract.cpp
    src/web/content/form/rename.cpp
    src/web/content/form.cpp
    src/web/content/form/widgets/problem.cpp
    src/web/filters/base64_binary.cpp
    src/bin/web.cpp
)

bunsan_web_add_skin(plain
    view/plain/base.tmpl
    view/plain/base_form.tmpl
    view/plain/insert.tmpl
    view/plain/extract.tmpl
    view/plain/rename.tmpl
    view/plain/existing.tmpl
    view/plain/available.tmpl
    view/plain/status.tmpl
    #view/plain/has_flag.tmpl
    #view/plain/set_flags.tmpl
    #view/plain/unset_flags.tmpl
    view/plain/clear_flags.tmpl
    view/plain/ignore.tmpl
    view/plain/info.tmpl
    view/plain/hash.tmpl
    view/plain/repack.tmpl
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME} ${PROJECT_NAME}_cli ${PROJECT_NAME}_web)
install(FILES ${proto_hdrs} DESTINATION include/bacs/archive/pb)
install(TARGETS plain DESTINATION lib/${PROJECT_NAME}/skins)

bunsan_include_tests()
