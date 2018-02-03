gen_stream_enum = executable('gen_stream_enum', 'stream_enum.cpp',
                             dependencies : bunsan_common_dep)
stream_enum_src = custom_target('stream_enum_cpp',
                                output : 'stream_enum.cpp',
                                command : [gen_stream_enum, '@OUTPUT@'])
test_stream_enum = executable('test_stream_enum',
                              stream_enum_src,
                              dependencies : test_deps)
test('stream_enum', test_stream_enum, env : testenv)
