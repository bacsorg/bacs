srcs = [
    'src/file.cpp',
]

#protos = [
#    'include/bacs/file/path.proto',
#    'include/bacs/file/range.proto',

#    'include/bacs/process/resource.proto',
#    'include/bacs/process/source.proto',

#    'include/bacs/utility/archiver.proto',
#]

deps = [
    bunsan_common_dep,
    bacs_proto_dep,
]
deps_str = [
    'bacs_protobuf',
    'bunsan_common',
]

#bacs_common_pb_srcs = protoc_cpp_gen.process(
#    protos,
#    preserve_path_from : meson.current_source_dir() / 'include',
#)
#bacs_common_pb = static_library(
#    'bacs_common_pb', bacs_common_pb_srcs,
#    dependencies : dependency('protobuf'),
#    override_options : ['warning_level=1'],
#)
#bacs_common_pb_dep = declare_dependency(
#    link_whole : bacs_common_pb,
#    dependencies : dependency('protobuf'),
#    include_directories : [include, #bacs_common_pb.private_dir_include()],
#)
#deps += bacs_common_pb_dep

include = include_directories('include')

lib = library('bacs_common', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
)

pkg.generate(lib)

bacs_common_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bacs_common',
    description : 'bacs helpers',
    libraries : [lib] + deps,
    requires : deps_str,
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
