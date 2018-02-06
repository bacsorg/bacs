srcs = [
    'src/base64.cpp',
    'src/checksum.cpp',
]

deps = [
    bunsan_common_dep,
    dependency('botan-2'),
]

include = include_directories('include')

lib = library('bunsan_crypto', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
)

bunsan_crypto_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_crypto',
    description : 'bunsan crypto helpers',
    libraries : [lib] + deps,
    requires : 'bunsan_common',
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
