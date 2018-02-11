subdir('cppcms')

include = include_directories('include')

srcs = [
    'src/json.cpp',
    'src/mime_file.cpp',
]

deps = [
    bunsan_common_dep,
    cppcms_dep,
]

lib = library('bunsan_web', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
)

bunsan_web_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_web',
    description : 'bunsan web helpers',
    libraries : [lib] + deps,
    requires : 'bunsan_common',
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
