compiler = meson.get_compiler('cpp')

if (host_machine.system() == 'linux' or
    host_machine.system() == 'bsd' or
    host_machine.system() == 'cygwin')
  platform = 'unix'
elif host_machine.system() == 'windows'
  if compiler.get_id() == 'gcc'
    platform = 'mingw'
  else
    platform = 'win32'
  endif
else
  error('Platform is not supported')
endif

srcs = [
    'src/getenv.cpp',
    'src/' + platform + '/setenv.cpp',
    'src/environment.cpp',

    'src/filesystem/tempdir.cpp',
    'src/filesystem/tempfiles.cpp',
    'src/filesystem/tempfile.cpp',
    'src/filesystem/read_data.cpp',
    'src/filesystem/write_data.cpp',
]

deps = [
    dependency('boost', modules : [
        'system',
        'filesystem',
        'unit_test_framework',
    ]),
]

include = include_directories('include')

lib = library('bunsan_test', srcs,
    dependencies : deps,
    include_directories : include,
    install : get_option('install-bunsan-test'),
    version : meson.project_version(),
)

bunsan_test_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_test',
    description : 'bunsan unit test helpers',
    libraries : [lib] + deps,
    version : meson.project_version(),
)

if get_option('install-bunsan-test')
  install_subdir('include',
                 install_dir : get_option('includedir'),
                 strip_directory : true)
endif

subdir('tests')
