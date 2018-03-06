srcs = [
    'src/task/stream_channel.cpp',
]
protos = [
    'include/bunsan/broker/connection.proto',
    'include/bunsan/broker/protocol.proto',
    'include/bunsan/broker/rabbit/connection.proto',
]

deps = [
    bunsan_common_dep,
    bunsan_protobuf_dep,
    bunsan_crypto_dep,
]
deps_str = [
    'bunsan_common',
    'bunsan_protobuf',
    'bunsan_crypto',
]

# TODO install headers
bunsan_broker_pb_srcs = protoc_cpp_gen.process(
    protos,
    preserve_path_from : join_paths(meson.current_source_dir(),
                                    'include'),
)

bunsan_broker_pb = static_library(
    'bunsan_broker_pb', bunsan_broker_pb_srcs,
    dependencies : dependency('protobuf'),
)
bunsan_broker_pb_dep = declare_dependency(
    link_whole : bunsan_broker_pb,
    dependencies : dependency('protobuf'),
    include_directories : [
        include,
        bunsan_broker_pb.private_dir_include()
    ],
)
deps += bunsan_broker_pb_dep

include = include_directories('include')

lib = library('bunsan_broker', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
)

bunsan_broker_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_broker',
    description : 'bunsan broker helpers',
    libraries : [lib] + deps,
    requires : deps_str,
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
