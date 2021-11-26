srcs = [
    'src/lib/judge.cpp',
]

bin_srcs = [
    'src/bin/cli.cpp',
]

protos = [
    'include/bacs/external/contest.proto',
    'include/bacs/external/external.proto',
    'include/bacs/external/language.proto',
    'include/bacs/external/result.proto',
    'include/bacs/external/submit.proto',
]

# FIXME generate gRPC also
bacs_external_pb_srcs = protoc_cpp_gen.process(
    protos,
    preserve_path_from : meson.current_source_dir() / 'include',
)
bacs_external_pb = static_library(
    'bacs_external_pb', bacs_external_pb_srcs,
    dependencies : dependency('protobuf'),
)
bacs_external_pb_dep = declare_dependency(
    link_whole : bacs_external_pb,
    dependencies : dependency('protobuf'),
    include_directories : [include, bacs_external_pb.private_dir_include()],
)

deps = [
    bunsan_common_dep,
    bunsan_curl_dep,
    bunsan_protobuf_dep,
    bacs_external_pb_dep,
]

include = include_directories('include')

lib = library('bacs_external', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
)

bacs_external_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

bin = executable('bacs_external_cli', bin_srcs,
    dependencies : bacs_external_dep,
    include_directories : include,
    install : true,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bacs_external',
    description : 'bacs helpers',
    libraries : [lib] + deps,
    requires : [
        'bunsan_common',
        'bunsan_curl',
        'bunsan_protobuf',
    ],
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
