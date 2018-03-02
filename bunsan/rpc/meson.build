srcs = [
    'src/error.cpp',
    'src/status.cpp',
]

deps = [
    bunsan_common_dep,
    bunsan_protobuf_dep,
    dependency('grpc'),
    dependency('grpc++'),
]
deps_str = [
    'bunsan_common',
    'bunsan_protobuf',
    # 'grpc', 'grpc++',
]

include = include_directories('include')

lib = library('bunsan_rpc', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
)

bunsan_rpc_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_rpc',
    description : 'bunsan rpc helpers',
    libraries : [lib] + deps,
    requires : deps_str,
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
