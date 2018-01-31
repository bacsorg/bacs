function(bunsan_add_configured_file)
    string(SHA1 call_hash "${ARGN}")
    set(script_stage_1 ${CMAKE_CURRENT_BINARY_DIR}/${call_hash}_stage_1.cmake)
    set(script_stage_2 ${CMAKE_CURRENT_BINARY_DIR}/${call_hash}_stage_2.cmake)

    set(options)
    set(one_value_args INPUT OUTPUT)
    set(multi_value_args VARIABLES DEPENDS)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(BUNSAN_CONFIGURE_FILE_INPUT ${ARG_INPUT})
    set(BUNSAN_CONFIGURE_FILE_OUTPUT ${ARG_OUTPUT})
    set(BUNSAN_CONFIGURE_FILE_VARIABLES)
    foreach(var ${ARG_VARIABLES} BUNSAN_CONFIGURE_FILE_INPUT BUNSAN_CONFIGURE_FILE_OUTPUT)
        set(BUNSAN_CONFIGURE_FILE_VARIABLES
            "${BUNSAN_CONFIGURE_FILE_VARIABLES}set(${var} \"@${var}@\")\n")
    endforeach()

    configure_file(${BunsanCMake_MODULE_ROOT}/ConfigureFile.cmake.in ${script_stage_1} @ONLY)
    configure_file(${script_stage_1} ${script_stage_2} @ONLY ESCAPE_QUOTES)
    bunsan_add_script_command(
        OUTPUT ${ARG_OUTPUT}
        SCRIPT ${script_stage_2}
        DEPENDS
            ${ARG_INPUT}
            ${ARG_DEPENDS}
    )
endfunction()
