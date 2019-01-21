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

    'src/single/driver.cpp',
    'src/single/generator.cpp',

    'src/single/test/list_storage.cpp',
    'src/single/test/matcher.cpp',
    'src/single/test/storage.cpp',

    'src/single/drivers/simple0/driver.cpp',
    'src/single/drivers/simple0/tests.cpp',

    'src/single/drivers/polygon_codeforces_com/driver.cpp',
    'src/single/drivers/polygon_codeforces_com/statement.cpp',
    'src/single/drivers/polygon_codeforces_com/tests.cpp',

    'src/single/generators/internal0.cpp',

    'src/single/importer.cpp',
]

protos = [
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
]

deps = [
    bunsan_common_dep,
    bunsan_process_dep,
    bunsan_pm_dep,
    bunsan_protobuf_dep,
    bacs_common_dep,
]
deps_str = [
    'bunsan_common',
    'bunsan_process',
    'bunsan_pm',
    'bunsan_protobuf',
    'bacs_common',
]

# FIXME use better dependency mechanism
protoc_cpp_gen = generator(protoc,
    output    : ['@BASENAME@.pb.cc',
                 '@BASENAME@.pb.h'],
    arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/../common/include',
                 '--proto_path=@CURRENT_SOURCE_DIR@/include',
                 '--cpp_out=@BUILD_DIR@', '@INPUT@'])
bacs_problem_pb_srcs = protoc_cpp_gen.process(
    protos,
    preserve_path_from : meson.current_source_dir() / 'include',
)
bacs_problem_pb = static_library(
    'bacs_problem_pb', bacs_problem_pb_srcs,
    dependencies : [
        dependency('protobuf'),
        bacs_common_pb_dep,
    ],
    override_options : ['warning_level=1'],
)
bacs_problem_pb_dep = declare_dependency(
    link_whole : bacs_problem_pb,
    dependencies : [
        dependency('protobuf'),
        bacs_common_pb_dep,
    ],
    include_directories : [include, bacs_problem_pb.private_dir_include()],
)
deps += bacs_problem_pb_dep

include = include_directories('include')

lib = library('bacs_problem', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
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
    requires : deps_str,
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
