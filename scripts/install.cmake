# This module must support manual loading for bootstrapping.
# All dependencies should be listed explicitly.
include(${CMAKE_CURRENT_LIST_DIR}/install_dirs.cmake)

set(BUNSAN_NEED_TARGETS_EXPORT NO CACHE INTERNAL "")

macro(bunsan_install_targets)
    if(${ARGC} GREATER 0)
        set(BUNSAN_NEED_TARGETS_EXPORT YES CACHE INTERNAL "")
    endif()
    bunsan_targets_finish_setup(${ARGN})
    install(TARGETS ${ARGN}
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endmacro()

include(CMakePackageConfigHelpers)
function(bunsan_install_project)
    if(BUNSAN_NEED_TARGETS_EXPORT)
        if(PROJECT_NAME STREQUAL bunsan_cmake)
            set(PROJECT_MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
        else()
            set(PROJECT_MODULE_ROOT ${BunsanCMake_MODULE_ROOT})
        endif()

        bunsan_get_package_install_path(PROJECT_ROOT ${PROJECT_NAME})
        set(PROJECT_CONFIG ${PROJECT_NAME}Config.cmake)
        set(PROJECT_TARGETS ${PROJECT_NAME}Targets)
        set(PROJECT_TARGETS_CONFIG ${PROJECT_TARGETS}.cmake)
        set(PROJECT_TARGETS_INCLUDE ${PROJECT_ROOT}/${PROJECT_TARGETS_CONFIG})
        configure_package_config_file(
            ${PROJECT_MODULE_ROOT}/Project.cmake.in ${PROJECT_CONFIG}
            INSTALL_DESTINATION ${PROJECT_ROOT}
            PATH_VARS
                PROJECT_ROOT
                PROJECT_CONFIG
                PROJECT_TARGETS_INCLUDE
        )
        install(EXPORT ${PROJECT_TARGETS}
                DESTINATION ${PROJECT_ROOT})
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG}
                DESTINATION ${PROJECT_ROOT})
    endif()

    # local exports
    if(BUNSAN_NEED_TARGETS_EXPORT)
        export(EXPORT ${PROJECT_TARGETS}
               FILE ${PROJECT_TARGETS_CONFIG})
    endif()
    export(PACKAGE ${PROJECT_NAME})
endfunction()

macro(bunsan_install_programs)
    install(PROGRAMS ${ARGN} DESTINATION ${CMAKE_INSTALL_BINDIR})
endmacro()

macro(bunsan_install_headers)
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endmacro()

include(GenerateExportHeader)

set(BUNSAN_EXPORT_INCLUDE bunsan_export_include)

function(bunsan_install_export_header target header)
    set(include_path ${CMAKE_CURRENT_BINARY_DIR}/${BUNSAN_EXPORT_INCLUDE})
    set(header_path ${include_path}/${header})
    generate_export_header(${target}
                           EXPORT_FILE_NAME ${header_path})
    set_target_properties(
        ${target}
        PROPERTIES
            VISIBILITY_INLINES_HIDDEN ON
            CXX_VISIBILITY_PRESET hidden
    )
    target_include_directories(${target} PRIVATE ${include_path})
    target_include_directories(${target} PUBLIC
        INTERFACE
            $<BUILD_INTERFACE:${include_path}>
    )
    install(DIRECTORY ${include_path}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endfunction()
