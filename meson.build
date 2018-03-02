project('bacs', 'cpp',
        version : '0.1.0',
        license : 'apache2',
        meson_version : '>=0.45',
        default_options : [
            'cpp_std=c++17',
            'warning_level=3',
            'b_asneeded=false', # plugins
        ])

# Compiler configuration
if (meson.get_compiler('cpp').get_id() == 'gcc' or
    meson.get_compiler('cpp').get_id() == 'clang')
  # plugins and backtrace
  add_global_link_arguments('-rdynamic', language : 'cpp')
  # Noisy protobuf workaround
  # TODO remove once meson supports warning suppression for generated code
  add_global_arguments('-Wno-unused-parameter', language : 'cpp')
endif

# Boost configuration, currently links dynamically only.
# FIXME it should probably be set by meson
add_global_arguments('-DBOOST_ALL_DYN_LINK', language : 'cpp')

subdir('bunsan')
subdir('bacs')

# FIXME this is workaround until generator can install files
meson.add_install_script('install-protos-workaround.py',
    'bunsan/broker/bunsan_broker_pb',
    'bacs/common/bacs_common_pb',
    'bacs/problem/bacs_problem_pb',
    'bacs/archive/bacs_archive_pb',
)
