find_package(Booster REQUIRED)

find_path(CPPCMS_INCLUDE_DIR cppcms/config.h)
find_library(CPPCMS_LIBRARY cppcms)
find_program(CPPCMS_TMPL_CC cppcms_tmpl_cc)

if(CPPCMS_INCLUDE_DIR AND CPPCMS_LIBRARY AND CPPCMS_TMPL_CC)
    set(CppCMS_INCLUDE_DIRS ${CPPCMS_INCLUDE_DIR} ${Booster_INCLUDE_DIRS})
    set(CppCMS_LIBRARIES ${CPPCMS_LIBRARY} ${Booster_LIBRARIES})
    set(CppCMS_TMPL_CC ${CPPCMS_TMPL_CC})
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CppCMS
    REQUIRED_VARS
        CppCMS_TMPL_CC
        CppCMS_INCLUDE_DIRS
        CppCMS_LIBRARIES
)

function(bunsan_web_add_skin target skin)
    set(srcs)
    foreach(src ${ARGN})
        list(APPEND srcs ${CMAKE_CURRENT_SOURCE_DIR}/${src})
    endforeach()
    set(cpp_target ${CMAKE_CURRENT_BINARY_DIR}/${target}_skin_cpp.cpp)
    add_custom_command(
        OUTPUT ${cpp_target}
        COMMAND ${CppCMS_TMPL_CC}
            -o ${cpp_target}
            ${srcs}
        DEPENDS ${srcs})
    bunsan_add_module_library(${target} ${cpp_target})
    set_target_properties(${target} PROPERTIES
                          OUTPUT_NAME ${skin})
endfunction()

function(bunsan_web_install_skins)
    bunsan_install_targets_to(${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/skins
        ${ARGN}
    )
endfunction()

function(bunsan_web_install_static)
    install(DIRECTORY static
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
endfunction()
