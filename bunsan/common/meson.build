compiler = meson.get_compiler('cpp')

srcs = [
    'src/tempfile.cpp',
    'src/error.cpp',
    'src/categorized_error.cpp',
    'src/system_error.cpp',
    'src/fnmatch.cpp',

    'src/application/application.cpp',
    'src/application/global_registry.cpp',

    'src/runtime/demangle.cpp',
    'src/runtime/stacktrace.cpp',

    'src/filesystem/error.cpp',
    'src/filesystem/operations.cpp',

    'src/log/core.cpp',
    'src/log/trivial.cpp',
]

deps = [
    compiler.find_library('dl', required : false), # boost::dll
    dependency('boost', modules : [
        'system',
        'filesystem',
        'log',
        'log_setup',
        'program_options',
        'regex',
        'serialization',
        'thread',
    ]),
]

include = include_directories('include')

lib = library('bunsan_common', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
)

bunsan_common_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_common',
    description : 'bunsan common helpers',
    libraries : [lib] + deps,
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
