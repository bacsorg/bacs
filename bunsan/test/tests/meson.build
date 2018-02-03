testenv = environment()
testenv.set('BUNSAN_SOURCE_DIR', meson.current_source_dir() + '/..')
testenv.set('BUNSAN_BINARY_DIR', meson.current_build_dir() + '/..')

test_deps = [
    bunsan_test_dep,
    dependency('boost', modules : [
        'unit_test_framework',
    ]),
]

test_prefix = 'bunsan_test_test_'
tests = [
    'env',
    'environment',
    'exec_test',
    'filesystem',
    'not_exec_test',
]

foreach t : tests
  e = executable(test_prefix + t,
                 t + '.cpp',
                 dependencies : test_deps)
  test(test_prefix + t, e, env : testenv)
endforeach
