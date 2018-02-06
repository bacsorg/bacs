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

include = include_directories('include')

lib = library('bunsan_pm', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
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

python3 = import('python3')
# FIXME this is workaround,
# see https://github.com/mesonbuild/meson/issues/3009
python_include_path = join_paths('/usr',
                                 python3.sysconfig_path('include'))
python_include = [
    include,
    include_directories(python_include_path, is_system : true),
]

python3.extension_module('bunsan_pm_python', python_srcs,
    dependencies : bunsan_pm_compat_dep,
    include_directories : python_include,
    install : true,
    install_dir : join_paths(bunsan_python_root, 'pm'),
)
install_data('src/compatibility/__init__.py',
    install_dir : join_paths(bunsan_python_root, 'pm'),
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_pm',
    description : 'bunsan package manager',
    libraries : [lib] + deps,
    requires : [
        'bunsan_common',
        'bunsan_crypto',
        'bunsan_utility'
    ],
    version : meson.project_version(),
)

pkg.generate(
    name : 'bunsan_pm_compatibility',
    description : 'bunsan package manager compatibility layer',
    libraries : [compat_lib, bunsan_pm_dep],
    requires : 'bunsan_pm',
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
