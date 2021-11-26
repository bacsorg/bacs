protos = [
    'include/bacs/archive/archive.proto',
    'include/bacs/archive/problem/error.proto',
    'include/bacs/archive/problem/flag.proto',
    'include/bacs/archive/problem/id.proto',
    'include/bacs/archive/problem/import.proto',
    'include/bacs/archive/problem/status.proto',
    
    'include/bacs/problem/problem.proto',
    'include/bacs/problem/result.proto',

    'include/bacs/problem/single/testing.proto',
    'include/bacs/problem/single/process/file.proto',
    'include/bacs/problem/single/process/settings.proto',
    'include/bacs/problem/single/process/execution.proto',
    'include/bacs/problem/single/task.proto',
    'include/bacs/problem/single/problem.proto',
    'include/bacs/problem/single/intermediate.proto',
    'include/bacs/problem/single/result.proto',
    
    'include/bacs/statement_provider/request.proto',
    
    'include/bacs/file/path.proto',
    'include/bacs/file/range.proto',

    'include/bacs/process/resource.proto',
    'include/bacs/process/source.proto',

    'include/bacs/utility/archiver.proto',
]

deps = [
    dependency('protobuf'),
    dependency('grpc++'),
    #bunsan_common_dep,
    #bunsan_crypto_dep,
    #bunsan_pm_dep,
    bunsan_protobuf_dep,
    #bunsan_rpc_dep,
    #bacs_problem_dep,
]
dep_str = [
    #'bunsan_common',
    #'bunsan_crypto',
    #'bunsan_pm',
    'bunsan_protobuf',
    #'bunsan_rpc',
    #'bacs_problem',
]
private_deps = []
cpp_args = []

# FIXME use better dependency mechanism
protoc_cpp_gen = generator(protoc,
    output    : ['@BASENAME@.pb.cc',
                 '@BASENAME@.pb.h'],
    arguments : [#'--proto_path=@CURRENT_SOURCE_DIR@/../common/include',
#                 '--proto_path=@CURRENT_SOURCE_DIR@/../problem/include',
                 '--proto_path=@CURRENT_SOURCE_DIR@/include',
                 '--cpp_out=@BUILD_DIR@', '@INPUT@'])
# FIXME use better plugin mechanism
grpc_cpp_plugin = find_program('grpc_cpp_plugin')
protoc_grpc_gen = generator(protoc,
    output    : ['@BASENAME@.grpc.pb.cc',
                 '@BASENAME@.grpc.pb.h'],
    arguments : [#'--proto_path=@CURRENT_SOURCE_DIR@/../common/include',
#                 '--proto_path=@CURRENT_SOURCE_DIR@/../problem/include',
                 '--proto_path=@CURRENT_SOURCE_DIR@/include',
                 '--plugin=protoc-gen-grpc=' + grpc_cpp_plugin.path(),
                 '--grpc_out=@BUILD_DIR@', '@INPUT@'])
                   
bacs_archive_pb_srcs = protoc_cpp_gen.process(
    protos,
    preserve_path_from : meson.current_source_dir() / 'include',
)
bacs_archive_grpc_srcs = protoc_grpc_gen.process(
    protos,
    preserve_path_from : meson.current_source_dir() / 'include',
)

bacs_archive_pb = static_library(
    'bacs_archive_pb',
    [bacs_archive_pb_srcs, bacs_archive_grpc_srcs],
    dependencies : [
        dependency('protobuf'),
        dependency('grpc++'),
        #bacs_problem_pb_dep,
    ],
    override_options : ['warning_level=1'],
)
bacs_archive_pb_dep = declare_dependency(
    link_whole : bacs_archive_pb,
    dependencies : [
        dependency('protobuf'),
        dependency('grpc++'),
        #bacs_common_pb_dep,
        #bacs_problem_pb_dep,
    ],
    include_directories : [
        include,
        bacs_archive_pb.private_dir_include(),
    ],
)
deps += bacs_archive_pb_dep

include = include_directories('include')

lib = library('bacs_protobuf', [],
    dependencies : [
        deps,
        private_deps,
    ],
    include_directories : include,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
)

bacs_proto_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)


pkg = import('pkgconfig')

pkg.generate(
    name : 'bacs_protobuf',
    description : 'bacs proto',
    libraries : [lib] + deps,
    requires : deps_str,
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

