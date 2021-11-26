bunsan_protobuf_test_pb_srcs = protoc_cpp_gen.process(
    'include/bunsan/protobuf/test/message.proto',
    preserve_path_from : meson.current_source_dir() / 'include',
)
bunsan_protobuf_test_pb = static_library(
    'bunsan_protobuf_test_pb', bunsan_protobuf_test_pb_srcs,
    dependencies : dependency('protobuf'),
    override_options : ['warning_level=1'],
)
bunsan_protobuf_test_pb_dep = declare_dependency(
    link_with : bunsan_protobuf_test_pb,
    dependencies : dependency('protobuf'),
    include_directories : [
        include,
        bunsan_protobuf_test_pb.private_dir_include()
    ],
)
