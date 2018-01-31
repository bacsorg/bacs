include(${CMAKE_CURRENT_LIST_DIR}/static_initializer_common.cmake)

function(bunsan_initializer_configure)
    bunsan_initializer_configure_common(${ARGN})
    bunsan_add_configured_file(
        INPUT ${BunsanCMake_MODULE_ROOT}/StaticInitializer.cpp.in
        OUTPUT ${ARG_OUTPUT}
        VARIABLES
            BUNSAN_STATIC_INITIALIZER_BODY
            BUNSAN_STATIC_INITIALIZER_EXTERNAL_INCLUDE
            BUNSAN_STATIC_INITIALIZER_FORWARD
            BUNSAN_STATIC_INITIALIZER_FUNCTION
            BUNSAN_STATIC_INITIALIZER_MODIFIER
        DEPENDS ${ARG_DEPENDS}
    )
endfunction()

function(bunsan_add_static_initializer_self target source)
    bunsan_static_initializer_name(${target} BUNSAN_STATIC_INITIALIZER_FUNCTION)
    set(source_dir ${CMAKE_CURRENT_BINARY_DIR}/${BUNSAN_STATIC_INITIALIZER_SOURCE})
    set(source_path ${source_dir}/${target}_self.cpp)
    set(script_path ${source_dir}/${target}_self.cmake)
    bunsan_static_initializer_external_header_name(
        ${target}
        BUNSAN_STATIC_INITIALIZER_EXTERNAL_HEADER
    )
    bunsan_static_initializer_external_name(
        ${target}
        BUNSAN_STATIC_INITIALIZER_EXTERNAL_CALL
    )
    set(BUNSAN_STATIC_INITIALIZER_OUTPUT ${source_path})
    set(BUNSAN_STATIC_INITIALIZER_SOURCES)

    foreach(source ${ARGN})
        if(NOT IS_ABSOLUTE ${source})
            set(source ${CMAKE_CURRENT_SOURCE_DIR}/${source})
        endif()
        get_filename_component(ext ${source} EXT)
        # skip headers
        if(NOT ext STREQUAL .h AND
           NOT ext STREQUAL .hpp)
            list(APPEND BUNSAN_STATIC_INITIALIZER_SOURCES ${source})
        endif()
    endforeach()

    configure_file(
        ${BunsanCMake_MODULE_ROOT}/AddStaticInitializerSelf.cmake.in
        ${script_path}
        @ONLY
    )

    bunsan_add_script_command(
        OUTPUT ${source_path}
        SCRIPT ${script_path}
        DEPENDS
            ${BUNSAN_STATIC_INITIALIZER_SOURCES}
            ${source_dir}/${BUNSAN_STATIC_INITIALIZER_EXTERNAL_HEADER}
    )

    set(${source} ${source_path} PARENT_SCOPE)
endfunction()

function(bunsan_add_static_initializer_self_call target source)
    bunsan_static_initializer_name(${target} BUNSAN_STATIC_INITIALIZER_FUNCTION)
    set(source_dir ${CMAKE_CURRENT_BINARY_DIR}/${BUNSAN_STATIC_INITIALIZER_SOURCE})
    set(source_path ${source_dir}/${target}_self_call.cpp)

    configure_file(${BunsanCMake_MODULE_ROOT}/StaticInitializerCall.cpp.in
                   ${source_path} @ONLY)

    set(${source} ${source_path} PARENT_SCOPE)
endfunction()

function(bunsan_add_static_initializer_external)
    foreach(target ${ARGN})
        bunsan_static_initializer_external_name(${target} function)
        set(source_dir ${CMAKE_CURRENT_BINARY_DIR}/${BUNSAN_STATIC_INITIALIZER_SOURCE})
        bunsan_static_initializer_external_header_name(${target} source_name)
        set(source_path ${source_dir}/${source_name})

        set(calls)
        get_target_property(dependencies ${target} INTERFACE_LINK_LIBRARIES)
        if(dependencies)
            foreach(dependency ${dependencies})
                if(TARGET ${dependency})
                    get_target_property(bunsan_target ${dependency} BUNSAN_TARGET)
                    if(bunsan_target)
                        bunsan_static_initializer_name(${dependency} call)
                        list(APPEND calls ${call})
                    endif()
                endif()
            endforeach()
        endif()

        bunsan_initializer_configure(
            OUTPUT ${source_path}
            MODIFIER "static"
            FUNCTION ${function}
            EXTERN_CALLS ${calls}
        )
    endforeach()
endfunction()
