# This module must support manual loading for bootstrapping.
# All dependencies should be listed explicitly.
include(${CMAKE_CURRENT_LIST_DIR}/install_dirs.cmake)

set(BUNSAN_${PROJECT_NAME}_TARGETS "${BUNSAN_${PROJECT_NAME}_TARGETS}" CACHE INTERNAL "")

function(bunsan_install_register_targets)
    foreach(target ${ARGN})
        list(FIND BUNSAN_${PROJECT_NAME}_TARGETS ${target} pos)
        if(pos EQUAL -1)
            list(APPEND BUNSAN_${PROJECT_NAME}_TARGETS ${target})
            set(BUNSAN_${PROJECT_NAME}_TARGETS "${BUNSAN_${PROJECT_NAME}_TARGETS}" CACHE INTERNAL "")
        endif()
    endforeach()
endfunction()

macro(bunsan_install_targets_prepare)
    bunsan_install_register_targets(${ARGN})
    bunsan_targets_finish_setup(${ARGN})
endmacro()

macro(bunsan_install_targets)
    bunsan_install_targets_prepare(${ARGN})
    install(TARGETS ${ARGN}
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endmacro()

macro(bunsan_install_targets_to destination)
    bunsan_install_targets_prepare(${ARGN})
    install(TARGETS ${ARGN}
        EXPORT ${PROJECT_NAME}Targets
        DESTINATION ${destination})
endmacro()

include(CMakePackageConfigHelpers)
function(bunsan_install_project)
    list(LENGTH BUNSAN_${PROJECT_NAME}_TARGETS length)
    if(length GREATER 0)
        set(need_export ON)
    else()
        set(need_export OFF)
    endif()

    if(need_export)
        bunsan_get_package_install_path(PROJECT_ROOT ${PROJECT_NAME})
        set(PROJECT_COMPONENTS ${BUNSAN_${PROJECT_NAME}_TARGETS})
        set(PROJECT_CONFIG ${PROJECT_NAME}Config.cmake)
        set(PROJECT_TARGETS ${PROJECT_NAME}Targets)
        set(PROJECT_TARGETS_CONFIG ${PROJECT_TARGETS}.cmake)
        set(PROJECT_TARGETS_INCLUDE ${PROJECT_ROOT}/${PROJECT_TARGETS_CONFIG})
        configure_package_config_file(
            ${BunsanCMake_MODULE_ROOT}/Project.cmake.in ${PROJECT_CONFIG}
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
    if(need_export)
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
