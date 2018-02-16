protos = [
    'include/bunsan/rpc/test/service.proto',
]
# FIXME use better dependency mechanism
protoc_cpp_gen = generator(protoc,
    output    : ['@BASENAME@.pb.cc',
                 '@BASENAME@.pb.h'],
    arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/include',
                 '--cpp_out=@BUILD_DIR@', '@INPUT@'])
# FIXME use better plugin mechanism
grpc_cpp_plugin = find_program('grpc_cpp_plugin')
protoc_grpc_gen = generator(protoc,
    output    : ['@BASENAME@.grpc.pb.cc',
                 '@BASENAME@.grpc.pb.h'],
    arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/include',
                 '--plugin=protoc-gen-grpc=' + grpc_cpp_plugin.path(),
                 '--grpc_out=@BUILD_DIR@', '@INPUT@'])
bunsan_rpc_test_pb_srcs = protoc_cpp_gen.process(
    protos,
    preserve_path_from : join_paths(meson.current_source_dir(),
                                    'include'),
)
bunsan_rpc_test_grpc_srcs = protoc_grpc_gen.process(
    protos,
    preserve_path_from : join_paths(meson.current_source_dir(),
                                    'include'),
)
bunsan_rpc_test_pb = static_library(
    'bunsan_rpc_test_pb',
    [bunsan_rpc_test_pb_srcs, bunsan_rpc_test_grpc_srcs],
    dependencies : [
        dependency('protobuf'),
        dependency('grpc++'),
    ],
)
bunsan_rpc_test_pb_dep = declare_dependency(
    link_with : bunsan_rpc_test_pb,
    dependencies : [
        dependency('protobuf'),
        dependency('grpc++'),
    ],
    include_directories : [
        bunsan_rpc_test_pb.private_dir_include(),
    ],
)
