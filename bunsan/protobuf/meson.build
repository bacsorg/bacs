srcs = [
    'src/base_io.cpp',
    'src/base_parser.cpp',
    'src/base_serializer.cpp',
    'src/binary_parser.cpp',
    'src/binary_serializer.cpp',
    'src/comparator.cpp',
    'src/text_parser.cpp',
    'src/text_serializer.cpp',
]

deps = [
    bunsan_common_dep,
    dependency('protobuf'),
]
deps_str = [
    'bunsan_common',
    # 'protobuf',
]

include = include_directories('include')

lib = library('bunsan_protobuf', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
)

bunsan_protobuf_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

protoc = find_program('protoc')
protoc_cpp_gen = generator(protoc,
    output    : ['@BASENAME@.pb.cc',
                 '@BASENAME@.pb.h'],
    arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/include',
                 '--cpp_out=@BUILD_DIR@', '@INPUT@'])

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_protobuf',
    description : 'bunsan protobuf helpers',
    libraries : [lib] + deps,
    requires : deps_str,
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
