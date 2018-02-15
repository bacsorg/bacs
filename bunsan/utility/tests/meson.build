testenv = environment()
testenv.set('BUNSAN_SOURCE_DIR', meson.current_source_dir() + '/..')
testenv.set('BUNSAN_BINARY_DIR', meson.current_build_dir() + '/..')

test_deps = [
    bunsan_test_dep,
    bunsan_utility_dep,
    dependency('boost', modules : [
        'unit_test_framework',
    ]),
]

prefix = 'bunsan_utility_test_'
tests = [
    'archiver',
    'fetcher',
    'maker',
    'mocks',
    'resolver',
]

foreach t : tests
  e = executable(prefix + t,
                 t + '.cpp',
                 dependencies : test_deps)
  test(prefix + t, e, env : testenv)
endforeach
