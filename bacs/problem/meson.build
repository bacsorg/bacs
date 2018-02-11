srcs = [
    'src/buildable.cpp',
    'src/utility.cpp',
    'src/id.cpp',
    'src/importer.cpp',
    'src/statement.cpp',
    'src/resource/parse.cpp',
    'src/system_verifier.cpp',

    'src/utilities/alias.cpp',
    'src/utilities/none.cpp',
    'src/utilities/single.cpp',
    'src/utilities/dot_net_solution.cpp',

    'src/statement_versions/copy.cpp',
    'src/statement_versions/pdflatex.cpp',
]

protos = [
    'include/bacs/problem/problem.proto',
    'include/bacs/problem/result.proto',
]

bacs_problem_pb_srcs = protoc_cpp_gen.process(
    protos,
    preserve_path_from : join_paths(meson.current_source_dir(),
                                    'include'),
)
bacs_problem_pb = static_library(
    'bacs_problem_pb', bacs_problem_pb_srcs,
    dependencies : dependency('protobuf'))
bacs_problem_pb_dep = declare_dependency(
    link_with : bacs_problem_pb,
    dependencies : [dependency('protobuf')],
    include_directories : [include, bacs_problem_pb.private_dir_include()],
)

deps = [
    bunsan_common_dep,
    bunsan_pm_dep,
    bunsan_protobuf_dep,
    bacs_problem_pb_dep,
]

include = include_directories('include')

lib = library('bacs_problem', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
)

bacs_problem_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bacs_problem',
    description : 'bacs problem helpers',
    libraries : [lib] + deps,
    requires : [
        'bunsan_common',
        'bunsan_pm',
        'bunsan_protobuf',
        'bacs_common',
    ],
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
