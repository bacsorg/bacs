srcs = [
    'src/easy.cpp',
    'src/easy_escape.cpp',
    'src/error_category.cpp',
    'src/global.cpp',
    'src/http_version.cpp',

    'src/http/client.cpp',
    'src/http/header.cpp',
    'src/http/header_parser.cpp',
    'src/http/header_set.cpp',
    'src/http/response_head.cpp',
    'src/http/status.cpp',

    'src/options/option_set.cpp',

    'src/detail/curl_delete.cpp',
    'src/detail/slist.cpp',
    'src/detail/string_list.cpp',
]

deps = [
    bunsan_common_dep,
    dependency('libcurl'),
    dependency('boost', modules : ['regex']),
]

include = include_directories('include')

lib = library('bunsan_curl', srcs,
    dependencies : deps,
    include_directories : include,
    install : true,
    version : meson.project_version(),
)

bunsan_curl_dep = declare_dependency(
    link_with : lib,
    include_directories : include,
    dependencies : deps,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bunsan_curl',
    description : 'bunsan curl helpers',
    libraries : [lib] + deps,
    requires : 'bunsan_common',
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
