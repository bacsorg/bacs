function(bunsan_configure_file input output)
    configure_file(${input} ${output} ${ARGN})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${output}
    )
endfunction()
