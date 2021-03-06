include = include_directories('include')

deps = [
    bunsan_common_dep,
    dependency('boost'),  # 'process'
]
deps_str = [
    'bunsan_common',
]

if (host_machine.system() == 'linux' or
    host_machine.system() == 'bsd' or
    host_machine.system() == 'cygwin')
  test_platform = 'unix'
  platform_srcs = [
      'src/process/unix/signal.cpp',

      'src/process/detail/unix/execute.cpp',
      'src/process/detail/unix/executor.cpp',
  ]
elif host_machine.system() == 'windows'
  test_platform = 'win32'
  platform_srcs = [
      'src/process/detail/win32/execute.cpp',
  ]
else
  error('Platform is not supported')
endif

srcs = [
    'src/process/context.cpp',
    'src/process/error.cpp',
    'src/process/execute.cpp',
    'src/process/executor.cpp',
    'src/process/native_executor.cpp',
    'src/process/boost_executor.cpp',
    'src/process/aliases.cpp',
    'src/process/path.cpp',

    'src/process/file/handle.cpp',

    'src/interprocess/sync/file_lock.cpp',
    'src/interprocess/sync/file_guard.cpp',
]

lib = library('bunsan_process', srcs + platform_srcs,
    include_directories : include,
    dependencies : deps,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
)

bunsan_process_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_process',
    description : 'bunsan::process library',
    libraries : [lib] + deps,
    requires : deps_str,
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
