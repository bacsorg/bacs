testenv = environment()
testenv.set('BUNSAN_SOURCE_DIR', meson.current_source_dir() + '/..')
testenv.set('BUNSAN_BINARY_DIR', meson.current_build_dir() + '/..')

test_deps = [
    turtle_dep,
    bunsan_common_dep,
    bunsan_test_dep,
    dependency('boost', modules : [
        'unit_test_framework',
    ]),
]

if (host_machine.system() == 'linux' or
    host_machine.system() == 'bsd' or
    host_machine.system() == 'cygwin')
  test_deps += [
      dependency('boost', modules : [
          'coroutine',
          'context',
          'thread',
      ]),
  ]
endif

test_prefix = 'bunsan_common_test_'
tests = [
    'application',
    'asio',
    'config',
    'error',
    'factory',
    'filesystem',
    'fnmatch',
    'forward_constructor',
    'get',
    'global_registry',
    'lazy_ptr',
    'log',
    'log_fallback',
    'memory',
    'plugin',
    'runtime',
    'serialization',
    'shared_cast',
    'tempfile',
]

foreach t : tests
  e = executable(test_prefix + t,
                 t + '.cpp',
                 dependencies : test_deps)
  test(test_prefix + t, e, env : testenv)
endforeach

subdir('generators')
