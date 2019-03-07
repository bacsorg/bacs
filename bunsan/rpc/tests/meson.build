testenv = environment()
testenv.set('BUNSAN_SOURCE_DIR', meson.current_source_dir() + '/..')
testenv.set('BUNSAN_BINARY_DIR', meson.current_build_dir() + '/..')

subdir('generators')

test_deps = [
    turtle_dep,
    bunsan_test_dep,
    bunsan_rpc_dep,
    bunsan_rpc_test_pb_dep,
    dependency('boost', modules : [
        'unit_test_framework',
    ]),
]

prefix = 'bunsan_rpc_test_'
tests = [
    'error',
    'overload',
    'status',
    'wrap',
]

foreach t : tests
  e = executable(prefix + t,
                 t + '.cpp',
                 dependencies : test_deps)
  test(prefix + t, e, env : testenv)
endforeach
