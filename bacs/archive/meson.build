srcs = [
    'src/lib/archive_client.cpp',
    'src/lib/archive_service.cpp',
    'src/lib/importer.cpp',
    'src/lib/repository.cpp',
    'src/lib/repository_impl.cpp',
    'src/lib/repository_post.cpp',
    'src/lib/revision.cpp',
    'src/lib/rpc.cpp',
    'src/lib/problem.cpp',
]
web_srcs = [
    'src/web/repository.cpp',
    'src/web/content/form/base.cpp',
    'src/web/content/form/list_query.cpp',
    'src/web/content/form/upload.cpp',
    'src/web/content/form/download.cpp',
    'src/web/content/form/rename.cpp',
    'src/web/content/form/existing.cpp',
    'src/web/content/form/status.cpp',
    'src/web/content/form/with_flag.cpp',
    'src/web/content/form/set_flags.cpp',
    'src/web/content/form/unset_flags.cpp',
    'src/web/content/form/import.cpp',
    'src/web/content/form.cpp',
    'src/web/content/form/widgets/archiver_config.cpp',
    'src/web/content/form/widgets/problem/id.cpp',
    'src/web/content/form/widgets/problem/id_set.cpp',
    'src/web/content/form/widgets/problem/optional_id_set.cpp',
    'src/web/content/form/widgets/problem/flag_set.cpp',
]
client_srcs = ['src/bin/client.cpp']
server_srcs = ['src/bin/server.cpp']
protos = [
    'include/bacs/archive/archive.proto',
    'include/bacs/archive/problem/error.proto',
    'include/bacs/archive/problem/flag.proto',
    'include/bacs/archive/problem/id.proto',
    'include/bacs/archive/problem/import.proto',
    'include/bacs/archive/problem/status.proto',
]

# FIXME use better dependency mechanism
protoc_cpp_gen = generator(protoc,
    output    : ['@BASENAME@.pb.cc',
                 '@BASENAME@.pb.h'],
    arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/../common/include',
                 '--proto_path=@CURRENT_SOURCE_DIR@/../problem/include',
                 '--proto_path=@CURRENT_SOURCE_DIR@/include',
                 '--cpp_out=@BUILD_DIR@', '@INPUT@'])
# FIXME use better plugin mechanism
grpc_cpp_plugin = find_program('grpc_cpp_plugin')
protoc_grpc_gen = generator(protoc,
    output    : ['@BASENAME@.grpc.pb.cc',
                 '@BASENAME@.grpc.pb.h'],
    arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/../common/include',
                 '--proto_path=@CURRENT_SOURCE_DIR@/../problem/include',
                 '--proto_path=@CURRENT_SOURCE_DIR@/include',
                 '--plugin=protoc-gen-grpc=' + grpc_cpp_plugin.path(),
                 '--grpc_out=@BUILD_DIR@', '@INPUT@'])
bacs_archive_pb_srcs = protoc_cpp_gen.process(
    protos,
    preserve_path_from : join_paths(meson.current_source_dir(),
                                    'include'),
)
bacs_archive_grpc_srcs = protoc_grpc_gen.process(
    protos,
    preserve_path_from : join_paths(meson.current_source_dir(),
                                    'include'),
)
bacs_archive_pb = static_library(
    'bacs_archive_pb',
    [bacs_archive_pb_srcs, bacs_archive_grpc_srcs],
    dependencies : [
        dependency('protobuf'),
        dependency('grpc++'),
        bacs_problem_pb_dep,
    ],
    override_options : ['warning_level=1'],
)
bacs_archive_pb_dep = declare_dependency(
    link_with : bacs_archive_pb,
    dependencies : [
        dependency('protobuf'),
        dependency('grpc++'),
        bacs_common_pb_dep,
        bacs_problem_pb_dep,
    ],
    include_directories : [
        include,
        bacs_archive_pb.private_dir_include(),
    ],
)

deps = [
    bunsan_common_dep,
    bunsan_crypto_dep,
    bunsan_pm_dep,
    bunsan_protobuf_dep,
    bunsan_rpc_dep,
    bacs_problem_dep,
    bacs_archive_pb_dep,
]

include = include_directories('include')

lib = library('bacs_archive', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
)

bacs_archive_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

bacs_archive_server = executable('bacs_archive_server', server_srcs,
    dependencies : bacs_archive_dep,
    install : true,
)

bacs_archive_client = executable('bacs_archive_client', client_srcs,
    dependencies : bacs_archive_dep,
    install : true,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bacs_archive',
    description : 'bacs archive',
    libraries : [lib] + deps,
    requires : ['bacs_common', 'bacs_problem'],
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
