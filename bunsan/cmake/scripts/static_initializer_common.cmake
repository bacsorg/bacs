# Note: must not depend on BunsanCMake.
include(CMakeParseArguments)

set(BUNSAN_STATIC_INITIALIZER_REGEX "BUNSAN_STATIC_INITIALIZER\\(([0-9a-zA-Z_]*),")
set(BUNSAN_STATIC_INITIALIZER_BODY_INDENT "    ")
set(BUNSAN_STATIC_INITIALIZER_SOURCE bunsan_static_initializer)

macro(bunsan_static_initializer_name target name)
    string(MAKE_C_IDENTIFIER ${target}_initializer ${name})
endmacro()

macro(bunsan_static_initializer_external_name target name)
    bunsan_static_initializer_name(${target} ${name})
    set(${name} ${${name}}_external)
endmacro()

macro(bunsan_static_initializer_external_header_name target name)
    set(${name} ${target}_external.hpp)
endmacro()

macro(bunsan_initializer_configure_common)
    set(options)
    set(one_value_args EXTERNAL_INCLUDE FUNCTION MODIFIER OUTPUT)
    set(multi_value_args CALLS DEPENDS EXTERN_CALLS)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(BUNSAN_STATIC_INITIALIZER_FUNCTION ${ARG_FUNCTION})
    set(BUNSAN_STATIC_INITIALIZER_FORWARD)
    set(BUNSAN_STATIC_INITIALIZER_MODIFIER ${ARG_MODIFIER})
    if(ARG_EXTERNAL_INCLUDE)
        set(BUNSAN_STATIC_INITIALIZER_EXTERNAL_INCLUDE
            "#include \"${ARG_EXTERNAL_INCLUDE}\"")
    endif()
    set(BUNSAN_STATIC_INITIALIZER_BODY "{")
    foreach(call ${ARG_EXTERN_CALLS})
        string(CONCAT BUNSAN_STATIC_INITIALIZER_FORWARD
            "${BUNSAN_STATIC_INITIALIZER_FORWARD}\n"
            "extern \"C\" void ${call}();"
        )
    endforeach()
    foreach(call ${ARG_CALLS} ${ARG_EXTERN_CALLS})
        string(CONCAT BUNSAN_STATIC_INITIALIZER_BODY
            "${BUNSAN_STATIC_INITIALIZER_BODY}\n"
            "${BUNSAN_STATIC_INITIALIZER_BODY_INDENT}${call}();"
        )
    endforeach()
    set(BUNSAN_STATIC_INITIALIZER_BODY "${BUNSAN_STATIC_INITIALIZER_BODY}\n}")
endmacro()
