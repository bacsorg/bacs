find_program(BUNSAN_GO_TOOL go)
set(BUNSAN_GO_FLAGS "" CACHE STRING "")
set(BUNSAN_GO_LINK_FLAGS "" CACHE STRING "")

set(bunsan_go_stage_dir ${CMAKE_BINARY_DIR}/bunsan_golang)
set(bunsan_go_objdir ${bunsan_go_stage_dir}/obj)
set(bunsan_go_libdir ${bunsan_go_stage_dir}/lib)

macro(bunsan_go_init)
    # Stage is global since packages are considered global
    file(MAKE_DIRECTORY ${bunsan_go_stage_dir})
    file(MAKE_DIRECTORY ${bunsan_go_libdir})
    file(MAKE_DIRECTORY ${bunsan_go_objdir})
endmacro()

macro(bunsan_make_go_target_name NAME PATH)
    string(REGEX REPLACE "[^0-9a-zA-Z_]" "__" ${NAME} "${PATH}")
endmacro()

function(bunsan_add_go_object PATH SOURCE OBJECT)
    get_filename_component(basename ${SOURCE} NAME_WE)
    if(NOT IS_ABSOLUTE ${SOURCE})
        set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE})
    endif()
    set(objdir ${bunsan_go_objdir}/${PATH})
    file(MAKE_DIRECTORY ${objdir})
    set(object ${objdir}/${basename}${CMAKE_C_OUTPUT_EXTENSION})
    add_custom_command(OUTPUT ${object}
        COMMAND ${BUNSAN_GO_TOOL} tool compile
            -I ${bunsan_go_libdir}
            ${BUNSAN_GO_FLAGS}
            -o ${object}
            ${SOURCE}
    )
    set(${OBJECT} ${object} PARENT_SCOPE)
endfunction()

function(bunsan_add_go_objects PATH OBJECTS)
    set(objects)
    foreach(source ${ARGN})
        bunsan_add_go_object(${PATH} ${source} object)
        list(APPEND objects ${object})
    endforeach()
    set(${OBJECTS} ${objects} PARENT_SCOPE)
endfunction()

function(bunsan_add_go_package PATH)
    bunsan_go_init()

    bunsan_make_go_target_name(target_name ${PATH})
    get_filename_component(dirname ${PATH} DIRECTORY)
    get_filename_component(filename ${PATH} NAME)

    bunsan_add_go_objects(${PATH} objects ${ARGN})

    set(libdir ${bunsan_go_libdir}/${dirname})
    file(MAKE_DIRECTORY ${libdir})
    set(library ${libdir}/${filename}${CMAKE_STATIC_LIBRARY_SUFFIX})
    add_custom_command(OUTPUT ${library}
        COMMAND ${BUNSAN_GO_TOOL} tool pack c ${library} ${objects}
        DEPENDS ${objects}
    )
    add_custom_target(${target_name} ALL DEPENDS ${library})
endfunction()

function(bunsan_add_go_executable TARGET PATH)
    bunsan_go_init()

    bunsan_add_go_objects(${PATH} objects ${ARGN})

    set(executable ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})
    if(WIN32)
        set(executable ${executable}.exe)
    endif()

    add_custom_command(OUTPUT ${executable}
        COMMAND ${BUNSAN_GO_TOOL} tool link
            -L ${bunsan_go_libdir}
            -o ${executable}
            ${objects}
        DEPENDS ${objects}
    )
    # TODO library dependencies
    add_custom_target(${TARGET} ALL DEPENDS ${executable})
endfunction()
