srcs = ['src/web/server.cpp']
server_srcs = ['src/bin/web.cpp']
protos = [
    'include/bacs/statement_provider/request.proto',
]

# FIXME use better dependency mechanism
protoc_cpp_gen = generator(protoc,
    output    : ['@BASENAME@.pb.cc',
                 '@BASENAME@.pb.h'],
    arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/../common/include',
                 '--proto_path=@CURRENT_SOURCE_DIR@/../problem/include',
                 '--proto_path=@CURRENT_SOURCE_DIR@/include',
                 '--cpp_out=@BUILD_DIR@', '@INPUT@'])
bacs_statement_provider_pb_srcs = protoc_cpp_gen.process(
    protos,
    preserve_path_from : join_paths(meson.current_source_dir(),
                                    'include'),
)
bacs_statement_provider_pb = static_library(
    'bacs_statement_provider_pb',
    bacs_statement_provider_pb_srcs,
    dependencies : [
        dependency('protobuf'),
        bacs_problem_pb_dep,
    ],
)
bacs_statement_provider_pb_dep = declare_dependency(
    link_with : bacs_statement_provider_pb,
    dependencies : [
        dependency('protobuf'),
        bacs_common_pb_dep,
        bacs_problem_pb_dep,
    ],
    include_directories : [
        include,
        bacs_statement_provider_pb.private_dir_include(),
    ],
)

deps = [
    dependency('botan-2'),
    cppcms_dep,
    bunsan_common_dep,
    bunsan_pm_dep,
    bacs_statement_provider_pb_dep,
]

include = include_directories('include')

lib = static_library('bacs_statement_provider', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
)

bacs_statement_provider_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

bacs_statement_provider_server = executable(
    'bacs_statement_provider_server', server_srcs,
    dependencies : bacs_statement_provider_dep,
    install : true,
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
