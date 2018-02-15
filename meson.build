project('bacs', 'cpp',
        version : '0.1.0',
        license : 'apache2',
        meson_version : '>=0.45',
        default_options : ['cpp_std=c++17',
                           'warning_level=3'])

# Compiler configuration
if (meson.get_compiler('cpp').get_id() == 'gcc' or
    meson.get_compiler('cpp').get_id() == 'clang')
  # plugins and backtrace
  add_global_link_arguments('-rdynamic', language : 'cpp')
  add_global_link_arguments('-Wl,--no-as-needed', language : 'cpp')
  # Noisy protobuf workaround
  # TODO remove once meson supports warning suppression for generated code
  add_global_arguments('-Wno-unused-parameter', language : 'cpp')
endif

# Boost configuration
default_library = get_option('default_library')
if default_library == 'shared'
  add_global_arguments('-DBOOST_ALL_DYN_LINK', language : 'cpp')
endif

subdir('bunsan')
subdir('bacs')
