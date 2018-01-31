# This module must support manual loading for bootstrapping.
# All dependencies should be listed explicitly.
include(${CMAKE_CURRENT_LIST_DIR}/install.cmake)

include(CMakePackageConfigHelpers)
include(CMakeParseArguments)

function(bunsan_install_module_)
    set(options)
    set(one_value_args NAME TEMPLATE BOOTSTRAP)
    set(multi_value_args DEPENDENCIES DIRECTORIES FILES INCLUDES SCRIPTS)
    cmake_parse_arguments(MODULE "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    bunsan_get_package_install_path(MODULE_ROOT ${MODULE_NAME})
    set(MODULE_SCRIPTS_DIR ${MODULE_ROOT}/scripts)
    if(MODULE_BOOTSTRAP)
        set(MODULE_BOOTSTRAP "include(\${${MODULE_NAME}_MODULE_SCRIPTS_DIR}/${MODULE_BOOTSTRAP})")
    endif()
    set(MODULE_CONFIG ${MODULE_NAME}Config.cmake)

    if(NOT MODULE_NAME STREQUAL BunsanCMake)
        list(INSERT MODULE_DEPENDENCIES 0 BunsanCMake)
    endif()
    set(MODULE_DEPENDENCIES_LOAD)
    foreach(dependency ${MODULE_DEPENDENCIES})
        set(MODULE_DEPENDENCIES_LOAD "${MODULE_DEPENDENCIES_LOAD}
if(NOT ${dependency}Loaded)
    if(${dependency}Builtin)
        ${dependency}Load()
    else()
        find_package(${dependency} CONFIG REQUIRED)
    endif()
endif()")
    endforeach()

    foreach(dir ${MODULE_DIRECTORIES})
        install(DIRECTORY ${dir} DESTINATION ${MODULE_SCRIPTS_DIR})
    endforeach()
    install(FILES ${MODULE_FILES} DESTINATION ${MODULE_ROOT})
    set(MODULE_INCLUDE_NAMES)
    foreach(dir ${MODULE_INCLUDES})
        install(DIRECTORY ${dir} DESTINATION ${MODULE_ROOT})
        get_filename_component(name ${dir} NAME)
        list(APPEND MODULE_INCLUDE_NAMES ${name})
    endforeach()
    install(FILES ${MODULE_SCRIPTS} DESTINATION ${MODULE_SCRIPTS_DIR})
    configure_package_config_file(
        ${MODULE_TEMPLATE} ${MODULE_CONFIG}
        INSTALL_DESTINATION ${MODULE_ROOT}
        PATH_VARS
            MODULE_ROOT
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_CONFIG}
            DESTINATION ${MODULE_ROOT})

    configure_file(
        ${BunsanCMake_MODULE_ROOT}/ModuleBuiltin.cmake.in ${MODULE_NAME}Builtin.cmake
        @ONLY
    )
    include(${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}Builtin.cmake)

    # local exports
    file(COPY ${MODULE_DIRECTORIES} ${MODULE_SCRIPTS}
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/scripts)
    file(COPY ${MODULE_FILES}
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${MODULE_INCLUDES}
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    export(PACKAGE ${MODULE_NAME})
endfunction()

# Should only be used by BunsanCMake itself
function(bunsan_bootstrap_module)
    set(BunsanCMake_MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
    bunsan_install_module_(BOOTSTRAP module.cmake ${ARGN})
endfunction()

function(bunsan_install_module)
    bunsan_install_module_(TEMPLATE ${BunsanCMake_MODULE_ROOT}/Module.cmake.in ${ARGN})
endfunction()

macro(bunsan_load_module_scripts)
    foreach(module ${ARGN})
        message(STATUS "Loading ${module}")
        include(${module})
    endforeach()
endmacro()

macro(bunsan_load_modules)
    foreach(arg ${ARGN})
        if(NOT IS_ABSOLUTE ${arg})
            set(arg ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
        endif()
        if(IS_DIRECTORY ${arg})
            # Make name pretty
            string(REGEX REPLACE "^(.*[^/])/*$" "\\1" dir "${arg}")
            message(STATUS "Loading modules from ${dir}")
            file(GLOB_RECURSE modules ${dir}/*.cmake)
            bunsan_load_module_scripts(${modules})
        else()
            bunsan_load_module_scripts(${arg})
        endif()
    endforeach()
endmacro()
