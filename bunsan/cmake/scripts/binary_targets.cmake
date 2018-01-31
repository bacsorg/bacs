include(${CMAKE_CURRENT_LIST_DIR}/static_initializer.cmake)

macro(bunsan_targets_finish_setup)
    foreach(target ${ARGN})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
            target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
            target_include_directories(${target} INTERFACE
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>
            )
        endif()
        if(NOT BUILD_SHARED_LIBS)
            target_compile_definitions(${target} PRIVATE BUNSAN_STATIC_INITIALIZER_GENERATOR=1)
        endif()
    endforeach()
    if(NOT BUILD_SHARED_LIBS)
        bunsan_add_static_initializer_external(${ARGN})
    endif()
endmacro()

function(bunsan_add_executable target)
    if(NOT BUILD_SHARED_LIBS)
        bunsan_add_static_initializer_self(${target} source ${ARGN})
        bunsan_add_static_initializer_self_call(${target} source_call)
    endif()
    add_executable(${target} ${source} ${source_call} ${ARGN})
    set_target_properties(${target} PROPERTIES BUNSAN_TARGET ON)
endfunction()

function(bunsan_require_static_library)
    list(FIND ARGN SHARED shared)
    if(NOT shared EQUAL -1)
        message(SEND_ERROR "Only static libraries are supported. Use bunsan_add_library().")
    endif()
endfunction()

function(bunsan_add_library target)
    if(NOT BUILD_SHARED_LIBS)
        bunsan_require_static_library(${ARGN})
        bunsan_add_static_initializer_self(${target} source ${ARGN})
    endif()
    add_library(${target} ${source} ${ARGN})
    set_target_properties(${target} PROPERTIES BUNSAN_TARGET ON)
endfunction()

function(bunsan_add_shared_library target)
    bunsan_add_library(${target} SHARED ${ARGN})
endfunction()

function(bunsan_add_static_library target)
    bunsan_add_library(${target} STATIC ${ARGN})
endfunction()

function(bunsan_add_module_library target)
    # Does not support BUNSAN_STATIC_INITIALIZER_GENERATOR.
    # This target is always PIC, and it may not depend on non-PIC (non-SHARED) libraries.
    add_library(${target} MODULE ${ARGN})
    set_target_properties(${target} PROPERTIES BUNSAN_TARGET ON)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)
        target_compile_options(${target} PUBLIC -fvisibility=hidden)
    endif()
endfunction()
