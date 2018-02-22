compiler = meson.get_compiler('cpp')

if (host_machine.system() == 'linux' or
    host_machine.system() == 'bsd' or
    host_machine.system() == 'cygwin')
  runtime_srcs = ['src/runtime/unix/stacktrace.cpp']
  runtime_deps = [compiler.find_library('dl')]
  if compiler.has_header('dlfcn.h')
    runtime_srcs += ['src/runtime/unix/stream-dlfcn.cpp']
  else
    runtime_srcs += ['src/runtime/unix/stream-fallback.cpp']
  endif
elif host_machine.system() == 'windows'
  runtime_srcs = ['src/runtime/win32/stacktrace.cpp']
  if compiler.get_id() == 'msvc'
    runtime_deps = [dependency('dbghelp')]
    runtime_srcs = ['src/runtime/win32/stream-msvc.cpp']
  else
    runtime_deps = [dependency('psapi')]
    runtime_srcs = ['src/runtime/win32/stream-fallback.cpp']
  endif
else
  runtime_srcs = ['src/runtime/fallback/stacktrace.cpp']
  runtime_deps = []
endif

srcs = [
    'src/tempfile.cpp',
    'src/error.cpp',
    'src/categorized_error.cpp',
    'src/system_error.cpp',
    'src/fnmatch.cpp',

    'src/application/application.cpp',
    'src/application/global_registry.cpp',

    'src/runtime/demangle.cpp',
    'src/runtime/detail/format.cpp',

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

lib = library('bunsan_common', srcs + runtime_srcs,
    dependencies : deps + runtime_deps,
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
