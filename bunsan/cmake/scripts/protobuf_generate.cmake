# TODO refactoring needed
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/install_dirs.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/install_python_module.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/libraries.cmake)

macro(bunsan_protobuf_get_target_includes includes target)
    get_target_property(${includes} ${target} INTERFACE_INCLUDE_DIRECTORIES)
    if(NOT includes)
        set(includes)
    endif()
endmacro()

macro(bunsan_protobuf_get_target_libraries libraries target)
    get_target_property(${libraries} ${target} INTERFACE_LINK_LIBRARIES)
    if(NOT libraries)
        set(libraries)
    endif()
endmacro()

macro(bunsan_protobuf_append_proto_paths var)
    foreach(proto_path ${ARGN})
        list(APPEND ${var} ${proto_path})
    endforeach()
    bunsan_list_remove_duplicates(${var})
endmacro()

macro(bunsan_protobuf_include_directories target visibility)
    if(${ARGC} GREATER 0)
        target_include_directories(${target} ${visibility} ${ARGN})
    endif()
endmacro()

macro(bunsan_protobuf_link_libraries target visibility)
    if(${ARGC} GREATER 0)
        target_link_libraries(${target} ${visibility} ${ARGN})
    endif()
endmacro()

function(bunsan_protobuf_get_libraries_proto_paths proto_paths)
    foreach(library ${ARGN})
        if(TARGET ${library})
            bunsan_protobuf_get_target_includes(includes ${library})
            bunsan_protobuf_append_proto_paths(proto_paths_ ${includes})
            bunsan_protobuf_get_target_libraries(libraries ${library})
            list(LENGTH libraries length)
            if(${length} GREATER 0)
                bunsan_protobuf_get_libraries_proto_paths(paths ${libraries})
                list(APPEND proto_paths_ ${paths})
            endif()
        endif()
    endforeach()
    set(${proto_paths} ${proto_paths_} PARENT_SCOPE)
endfunction()

# OUTPUT is list
function(bunsan_protobuf_get_names)
    set(options)
    set(one_value_args PLUGIN PROTO
        OUTPUT PREFIX NAME
    )
    set(multi_value_args)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    get_filename_component(filename_we ${ARG_PROTO} NAME_WE)
    get_filename_component(dirname ${ARG_PROTO} DIRECTORY)
    set(path_we ${dirname}/${filename_we})
    if(ARG_PLUGIN STREQUAL cpp)
        # make header first, used in bunsan_protobuf_install
        set(output ${path_we}.pb.h ${path_we}.pb.cc)
    elseif(ARG_PLUGIN STREQUAL grpc_cpp)
        # make header first, used in bunsan_protobuf_install
        set(output ${path_we}.grpc.pb.h ${path_we}.grpc.pb.cc)
    elseif(ARG_PLUGIN STREQUAL python)
        set(output ${path_we}_pb2.py)
    elseif(ARG_PLUGIN STREQUAL grpc_python)
        set(output) # merged with python
    elseif(ARG_PLUGIN STREQUAL go)
        set(output ${path_we}.pb.go)
    else()
        message(SEND_ERROR "Unknown protoc plugin ${ARG_PLUGIN}")
    endif()
    if(ARG_OUTPUT)
        set(${ARG_OUTPUT} ${output} PARENT_SCOPE)
    endif()
    if(ARG_PREFIX)
        set(${ARG_PREFIX} ${dirname} PARENT_SCOPE)
    endif()
    if(ARG_NAME)
        set(${ARG_NAME} ${filename_we} PARENT_SCOPE)
    endif()
endfunction()

function(bunsan_protobuf_install_headers PLUGIN BINARY_PREFIX PROTO)
    bunsan_protobuf_get_names(
        PLUGIN ${PLUGIN}
        PROTO ${PROTO}
        OUTPUT output
        PREFIX prefix
        NAME name
    )
    if(PLUGIN STREQUAL cpp OR
       PLUGIN STREQUAL grpc_cpp)
        # header
        list(GET output 0 header)
        install(FILES ${BINARY_PREFIX}/${header}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${prefix})
    elseif(PLUGIN STREQUAL python)
        # assumes that len(output) == 1
        bunsan_install_python_module_file(MODULE ${prefix}/${name}_pb2
                                          FILE ${BINARY_PREFIX}/${output})
    elseif(PLUGIN STREQUAL go)
        # does nothing
    else()
        message(SEND_ERROR "Unknown protoc plugin ${PLUGIN}")
    endif()
endfunction()

function(bunsan_add_protobuf_generate)
    bunsan_find_package(Protobuf REQUIRED)

    set(options INSTALL_HEADERS)
    set(one_value_args
        TARGET
        SOURCE_PREFIX BINARY_PREFIX
        OUTPUT
    )
    set(multi_value_args PLUGINS PROTOS PROTO_PATHS PLUGIN_PARAMS)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    file(MAKE_DIRECTORY ${ARG_BINARY_PREFIX})

    set(protoc_params)

    foreach(proto_path ${ARG_PROTO_PATHS})
        list(APPEND protoc_params --proto_path=${proto_path})
    endforeach()

    set(plugin_names)
    foreach(plugin ${ARG_PLUGINS})
        string(FIND ${plugin} "=" eqpos)
        set(plugin_params "")
        foreach(plugin_param ${ARG_PLUGIN_PARAMS})
            set(plugin_params "${plugin_params}${plugin_param}:")
        endforeach()
        string(SUBSTRING ${plugin} 0 ${eqpos} plugin_name)
        list(APPEND protoc_params
            "--plugin=protoc-gen-${plugin}"
            "--${plugin_name}_out=${plugin_params}${ARG_BINARY_PREFIX}"
        )
        list(APPEND plugin_names ${plugin_name})
    endforeach()

    set(sources)
    set(output)
    foreach(proto ${ARG_PROTOS})
        list(APPEND sources ${ARG_SOURCE_PREFIX}/${proto})
        foreach(plugin_name ${plugin_names})
            bunsan_protobuf_get_names(
                PLUGIN ${plugin_name}
                PROTO ${proto}
                OUTPUT proto_outs
            )
            foreach(proto_out ${proto_outs})
                list(APPEND output ${ARG_BINARY_PREFIX}/${proto_out})
            endforeach()
            if(ARG_INSTALL_HEADERS)
                    bunsan_protobuf_install_headers(
                        ${plugin_name} ${ARG_BINARY_PREFIX} ${proto})
            endif()
        endforeach()
    endforeach()

    add_custom_command(
        OUTPUT ${output}
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${protoc_params} ${sources}
        DEPENDS ${sources}
    )
    if(ARG_TARGET)
        add_custom_target(${ARG_TARGET} ALL DEPENDS ${output})
    endif()
    if(ARG_OUTPUT)
        set(${ARG_OUTPUT} ${output} PARENT_SCOPE)
    endif()
endfunction()

# bunsan_add_protobuf_cxx_library(
#     INSTALL install headers and library
#     PYTHON also build python library
#     STATIC build static library
#     SHARED build shared library
#     GRPC use gRPC plugin
#
#     TARGET target-name
#     INCLUDE_DIRECTORIES some include dirs
#     LIBRARIES some libraries (targets will be fetched for includes)
#     EXPORT_MACRO_NAME LIBRARY_EXPORT (dllexport code)
#     PLUGINS plugin1=exe plugin2=exe2
#     PROTOS *.proto
# )
function(bunsan_add_protobuf_cxx_library)
    bunsan_find_package(Protobuf REQUIRED)

    set(options INSTALL PYTHON STATIC SHARED GRPC)
    set(one_value_args TARGET EXPORT_MACRO_NAME)
    set(multi_value_args
        PROTOS PLUGINS
        INCLUDE_DIRECTORIES LIBRARIES
    )
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(ARG_STATIC)
        set(libtype STATIC)
    elseif(ARG_SHARED)
        set(libtype SHARED)
    endif()

    set(generate_params)
    if(ARG_INSTALL)
        list(APPEND generate_params INSTALL_HEADERS)
    endif()

    set(proto_paths)
    bunsan_protobuf_append_proto_paths(proto_paths
        ${ARG_INCLUDE_DIRECTORIES}
        ${CMAKE_CURRENT_SOURCE_DIR}/include # bunsan project layout
    )
    bunsan_protobuf_get_libraries_proto_paths(proto_paths_ ${ARG_LIBRARIES})
    list(APPEND proto_paths ${proto_paths_})

    set(cpp_params)
    if(ARG_EXPORT_MACRO_NAME)
        list(APPEND cpp_params "dllexport_decl=${ARG_EXPORT_MACRO_NAME}")
    endif()

    # C++
    set(cxx_plugins cpp)
    if(ARG_GRPC)
        find_package(Grpc REQUIRED)
        list(APPEND cxx_plugins grpc_cpp=${Grpc_CXX_PLUGIN})
    endif()
    bunsan_add_protobuf_generate(
        # TARGET
        PLUGINS ${cxx_plugins}
        SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/include
        BINARY_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/bunsan_protobuf/cxx
        PROTOS ${ARG_PROTOS}
        PROTO_PATHS ${proto_paths}
        PLUGIN_PARAMS ${cpp_params}
        OUTPUT cpp_output
        ${generate_params}
    )
    bunsan_add_library(${ARG_TARGET} ${libtype} ${cpp_output})
    bunsan_protobuf_include_directories(${ARG_TARGET} PUBLIC
        ${PROTOBUF_INCLUDE_DIRS}
        ${ARG_INCLUDE_DIRECTORIES}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/bunsan_protobuf/cxx>
    )
    bunsan_protobuf_link_libraries(${ARG_TARGET} PUBLIC
        ${PROTOBUF_LIBRARIES}
        ${ARG_LIBRARIES}
    )
    if(ARG_GRPC)
        target_include_directories(${ARG_TARGET} PUBLIC ${Grpc_INCLUDE_DIRS})
        target_link_libraries(${ARG_TARGET} PUBLIC ${Grpc_LIBRARIES})
    endif()

    if(ARG_PYTHON)
        set(python_plugins python)
        if(ARG_GRPC)
            list(APPEND python_plugins grpc_python=${Grpc_PYTHON_PLUGIN})
        endif()
        bunsan_add_protobuf_generate(
            TARGET ${ARG_TARGET}_python
            PLUGINS ${python_plugins}
            SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/include
            BINARY_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/bunsan_protobuf/python
            PROTOS ${ARG_PROTOS}
            PROTO_PATHS ${proto_paths}
            # PLUGIN_PARAMS
            # OUTPUT
            ${generate_params}
        )
        # Make sure that make TARGET also builds python
        add_dependencies(${ARG_TARGET} ${ARG_TARGET}_python)
    endif()

    # exports
    if(ARG_INSTALL)
        bunsan_install_targets(${ARG_TARGET})
    else()
        bunsan_targets_finish_setup(${ARG_TARGET})
    endif()
endfunction()
