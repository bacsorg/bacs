testenv = environment()
testenv.set('BUNSAN_SOURCE_DIR', meson.current_source_dir() + '/..')
testenv.set('BUNSAN_BINARY_DIR', meson.current_build_dir() + '/..')

test_deps = [
    turtle_dep,
    bunsan_test_dep,
    bunsan_process_dep,
    dependency('boost', modules : [
        'unit_test_framework',
    ]),
]

prefix = 'bunsan_process_test_'
tests = [
    'execute',
    'file_guard',
    'process',
]
platform_tests = [
    'process',
]

foreach t : tests
  e = executable(prefix + t,
                 t + '.cpp',
                 dependencies : test_deps)
  test(prefix + t, e, env : testenv)
endforeach

foreach t : platform_tests
  e = executable(prefix + test_platform + t,
                 join_paths(test_platform, t + '.cpp'),
                 dependencies : test_deps)
  test(prefix + test_platform + t, e, env : testenv)
endforeach
