srcs = [
    'src/error.cpp',
    'src/utility.cpp',
    'src/executor.cpp',

    'src/resolver.cpp',
    'src/custom_resolver.cpp',
    'src/system_resolver.cpp',

    'src/builder.cpp',
    'src/builders/cmake.hpp',
    'src/builders/cmake.cpp',
    'src/builders/conf_make_install.hpp',
    'src/builders/conf_make_install.cpp',

    'src/fetcher.cpp',
    'src/fetchers/curl.hpp',
    'src/fetchers/wget.cpp',
    'src/fetchers/wget.hpp',
    'src/fetchers/curl.cpp',
    'src/fetchers/copy.hpp',
    'src/fetchers/copy.cpp',

    'src/archiver.cpp',
    'src/archivers/tar.hpp',
    'src/archivers/cwd_split.cpp',
    'src/archivers/7z.hpp',
    'src/archivers/tar.cpp',
    'src/archivers/7z.cpp',
    'src/archivers/cwd_split.hpp',

    'src/maker.cpp',
    'src/makers/make.cpp',
    'src/makers/make.hpp',
]

deps = [
    bunsan_common_dep,
    bunsan_process_dep,
]

include = include_directories('include')

lib = shared_library('bunsan_utility', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
)

bunsan_utility_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_utility',
    description : 'bunsan utility helpers',
    libraries : [lib] + deps,
    requires : ['bunsan_common', 'bunsan_process'],
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
