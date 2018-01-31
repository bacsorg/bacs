function(bunsan_create_directories)
    foreach(dir ${ARGN})
        if(EXISTS ${dir})
            if(NOT IS_DIRECTORY ${dir})
                message(SEND_ERROR "${dir} should be a directory")
            endif()
        else()
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E make_directory ${dir}
                RESULT_VARIABLE result
            )
            if(NOT result EQUAL 0)
                message(SEND_ERROR "Unable to create directory \"${dir}\".")
            endif()
        endif()
    endforeach()
endfunction()
