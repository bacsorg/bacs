testenv = environment()
testenv.set('BUNSAN_SOURCE_DIR', meson.current_source_dir() + '/..')
testenv.set('BUNSAN_BINARY_DIR', meson.current_build_dir() + '/..')

test_deps = [
    bunsan_test_dep,
    bunsan_curl_dep,
    dependency('boost', modules : [
        'unit_test_framework',
    ]),
]

prefix = 'bunsan_curl_test_'
tests = [
    'callback',
    'easy',
    'http',
    'option_set',
    'slist',
    'traits',
    'wrapper',
]
server_tests = [
    'easy_info',
    'easy_options',
    'http_client',
]

foreach t : tests
  e = executable(prefix + t,
                 t + '.cpp',
                 dependencies : test_deps)
  test(prefix + t, e, env : testenv)
endforeach

python3 = import('python3')
py3bin = python3.find_python()

foreach t : server_tests
  e = executable(prefix + t,
                 t + '.cpp',
                 dependencies : test_deps)
  args = [
      join_paths(meson.current_source_dir(), 'resources', 'server.py'),
      e,
  ]
  test(prefix + t, py3bin, args : args, env : testenv)
endforeach
