testenv = environment()
testenv.set('BUNSAN_SOURCE_DIR', meson.current_source_dir() + '/..')
testenv.set('BUNSAN_BINARY_DIR', meson.current_build_dir() + '/..')

subdir('generators')

test_deps = [
    bunsan_test_dep,
    bunsan_protobuf_dep,
    bunsan_protobuf_test_pb_dep,
    dependency('boost', modules : [
        'unit_test_framework',
    ]),
]

prefix = 'bunsan_protobuf_test_'
tests = [
    'binary',
    'comparator',
    'initialization',
    'text',
]

foreach t : tests
  e = executable(prefix + t,
                 t + '.cpp',
                 dependencies : test_deps)
  test(prefix + t, e, env : testenv)
endforeach
