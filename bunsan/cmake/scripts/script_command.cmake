function(bunsan_add_script_command)
    set(options)
    set(one_value_args SCRIPT)
    set(multi_value_args DEPENDS OUTPUT)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    add_custom_command(
        OUTPUT ${ARG_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -P ${ARG_SCRIPT}
        DEPENDS
            ${ARG_SCRIPT}
            ${ARG_DEPENDS}
        VERBATIM
    )
endfunction()
