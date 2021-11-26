cppcms_tmpl_cc = find_program('cppcms_tmpl_cc')

plain_skin_srcs = [
    'plain/base.tmpl',
    'plain/base_form.tmpl',

    'plain/response/id_set.tmpl',
    'plain/response/import_map.tmpl',
    'plain/response/status.tmpl',
    'plain/response/status_map.tmpl',

    'plain/error.tmpl',
    'plain/upload.tmpl',
    'plain/download.tmpl',
    'plain/rename.tmpl',
    'plain/existing.tmpl',
    'plain/status.tmpl',
    'plain/with_flag.tmpl',
    'plain/set_flags.tmpl',
    'plain/unset_flags.tmpl',
    'plain/clear_flags.tmpl',
    'plain/import_result.tmpl',
    'plain/import.tmpl',
]
plain_skin_cpp = custom_target('bacs_archive_skin_plain_src',
    command : [cppcms_tmpl_cc, '-o', '@OUTPUT@', '-s', 'plain', '@INPUT@'],
    input : plain_skin_srcs,
    output : 'skin.cpp',
)
plain_skin_module = shared_module('plain', plain_skin_cpp,
    dependencies : bacs_archive_dep,
    install : true,
    install_dir : get_option('libdir') / 'bacs_archive' / 'skins',
)
