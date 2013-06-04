project(bunsan_bacs_archive)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanWeb)

bunsan_use_boost(system filesystem serialization unit_test_framework program_options thread)
bunsan_use_bunsan(common process utility pm bacs_problem)
bunsan_use_protobuf()

bunsan_protobuf_generate_cpp(SOURCES proto_srcs HEADERS proto_hdrs INSTALL include PROTOS
    bacs/archive/pb/problem.proto
)

bunsan_add_shared_library(${PROJECT_NAME}
    ${proto_srcs}

    src/lib/importer.cpp
    src/lib/repository.cpp
    src/lib/repository_impl.cpp
    src/lib/repository_repack.cpp
    src/lib/problem.cpp
)
bunsan_use(${PROJECT_NAME})

bunsan_add_executable(${PROJECT_NAME}_cli src/bin/cli.cpp)

bunsan_use_bunsan(network web)
bunsan_use(cppcms booster)
bunsan_add_executable(${PROJECT_NAME}_web
    src/web/repository.cpp
    src/web/content/form/base.cpp
    src/web/content/form/list_query.cpp
    src/web/content/form/insert.cpp
    src/web/content/form/extract.cpp
    src/web/content/form/rename.cpp
    src/web/content/form/existing.cpp
    src/web/content/form/available.cpp
    src/web/content/form/with_flag.cpp
    src/web/content/form/set_flags.cpp
    src/web/content/form/unset_flags.cpp
    src/web/content/form.cpp
    src/web/content/form/widgets/archiver_config.cpp
    src/web/content/form/widgets/problem/id.cpp
    src/web/content/form/widgets/problem/id_set.cpp
    src/web/content/form/widgets/problem/optional_id_set.cpp
    src/web/content/form/widgets/problem/flag_set.cpp
    src/web/filters/base64_binary.cpp
    src/bin/web.cpp
)

bunsan_web_add_skin(plain
    view/plain/base.tmpl
    view/plain/error.tmpl
    view/plain/base_form.tmpl
    view/plain/insert.tmpl
    view/plain/extract.tmpl
    view/plain/rename.tmpl
    view/plain/existing.tmpl
    view/plain/available.tmpl
    view/plain/status.tmpl
    view/plain/with_flag.tmpl
    view/plain/set_flags.tmpl
    view/plain/unset_flags.tmpl
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
install(DIRECTORY static DESTINATION share/${PROJECT_NAME})

bunsan_include_tests()
