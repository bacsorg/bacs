srcs = [
    'src/lib/index.cpp',
    'src/lib/entry.cpp',
    'src/lib/checksum.cpp',
    'src/lib/repository.cpp',
    'src/lib/snapshot.cpp',

    'src/lib/repository/builder.cpp',
    'src/lib/repository/cache.cpp',
    'src/lib/repository/distributor.cpp',
    'src/lib/repository/extractor.cpp',
    'src/lib/repository/local_system.cpp',

    'src/lib/cache.cpp',
]

compat_srcs = [
    'src/compatibility/repository_cpp.cpp',
    'src/compatibility/repository_c.cpp',
]

python_srcs = [
    'src/compatibility/repository_python.cpp',
]

bin_srcs = [
    'src/bin/cli.cpp',
]

deps = [
    bunsan_common_dep,
    bunsan_crypto_dep,
    bunsan_utility_dep,
]
deps_str = [
    'bunsan_common',
    'bunsan_crypto',
    'bunsan_utility',
]

include = include_directories('include')

lib = library('bunsan_pm', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
)

bunsan_pm_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

compat_lib = library('bunsan_pm_compatibility', compat_srcs,
    dependencies : bunsan_pm_dep,
    include_directories : include,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
)

bunsan_pm_compat_dep = declare_dependency(
    link_with : compat_lib,
    include_directories : include,
    dependencies : bunsan_pm_dep,
)

bin = executable('bunsan_pm_cli', bin_srcs,
    dependencies : bunsan_pm_dep,
    include_directories : include,
    install : true,
)

python3 = import('python').find_installation('python3')

python3.extension_module('_pm', python_srcs,
    dependencies : [
        python3.dependency(),
        bunsan_pm_compat_dep,
    ],
    include_directories : include,
    install : true,
    install_dir : bunsan_python_root / 'pm',
)
install_data('src/compatibility/__init__.py',
    install_dir : bunsan_python_root / 'pm',
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_pm',
    description : 'bunsan package manager',
    libraries : [lib] + deps,
    requires : deps_str,
    version : meson.project_version(),
)

pkg.generate(
    name : 'bunsan_pm_compatibility',
    description : 'bunsan package manager compatibility layer',
    libraries : [compat_lib, bunsan_pm_dep],
    requires : deps_str + ['bunsan_pm'],
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
