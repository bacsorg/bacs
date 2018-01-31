function(bunsan_get_install_python_path path)
    if(UNIX)
        set(python_version ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})
        set(${path} lib/python${python_version}/site-packages PARENT_SCOPE)
    elseif(WIN32)
        set(python_version ${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
        set(${path} Python${python_version}/Lib/site-packages PARENT_SCOPE)
    else()
        message(SEND_ERROR "Environment is not supported.")
    endif()
endfunction()

function(bunsan_get_install_python_package_components)
    set(options)
    set(one_value_args PACKAGE DIRECTORY NAME PATH)
    set(multi_value_args)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})
    bunsan_required_arguments(ARG PACKAGE)

    string(REPLACE "." "/" package ${ARG_PACKAGE})
    get_filename_component(package_name ${package} NAME)
    get_filename_component(package_directory ${package} DIRECTORY)

    bunsan_get_install_python_path(python_path)

    if(ARG_DIRECTORY)
        set(${ARG_DIRECTORY} ${python_path}/${package_directory} PARENT_SCOPE)
    endif()
    if(ARG_NAME)
        set(${ARG_NAME} ${package_name} PARENT_SCOPE)
    endif()
    if(ARG_PATH)
        set(${ARG_PATH} ${python_path}/${package} PARENT_SCOPE)
    endif()
endfunction()

macro(bunsan_install_python_module_common source_arg)
    set(options)
    set(one_value_args ${source_arg} MODULE)
    set(multi_value_args)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})
    bunsan_required_arguments(ARG ${one_value_args})

    bunsan_get_install_python_package_components(
        PACKAGE ${ARG_MODULE}
        DIRECTORY module_directory
        NAME module_name
        PATH module_path
    )

    string(REPLACE "/" ";" module_directory_split ${module_directory})
    list(LENGTH module_directory_split length)
    math(EXPR last "${length} - 1")
    list(REMOVE_AT module_directory_split last)
    set(base_rpath "$ORIGIN")
    foreach(part ${module_directory_split})
        set(base_rpath "${base_rpath}/..")
    endforeach()

    set(rpath "${base_rpath};${base_rpath}/../${CMAKE_INSTALL_LIBDIR};${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endmacro()

function(bunsan_install_python_module_target)
    bunsan_install_python_module_common(TARGET ${ARGN})

    set_target_properties(${ARG_TARGET}
        PROPERTIES
        OUTPUT_NAME ${module_name}
        PREFIX ""
        INSTALL_RPATH "${rpath}"
    )

    install(TARGETS ${ARG_TARGET} DESTINATION ${module_directory})
endfunction()

function(bunsan_install_python_module_compile_one pyfile)
    string(REPLACE ";" " " pyflags "${ARGN}")
    install(CODE "
        message(STATUS \"Compiling python source [${ARGN}]: ${pyfile}\")
        set(pyfile \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${pyfile}\")
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} ${pyflags} -m py_compile \${pyfile}
            RESULT_VARIABLE result
        )
        if(NOT result EQUAL 0)
            message(SEND_ERROR \"Unable to compile \${pyfile}\")
        endif()
    ")
endfunction()

function(bunsan_install_python_module_compile)
    foreach(pyfile ${ARGN})
        if(python_version VERSION_LESS 2.7)
            bunsan_install_python_module_compile_one(${pyfile})
        endif()
        bunsan_install_python_module_compile_one(${pyfile} -OO)
    endforeach()
endfunction()

function(bunsan_install_python_module_init)
    bunsan_install_python_module_common(INIT ${ARGN})

    install(FILES ${ARG_INIT} DESTINATION ${module_path} RENAME __init__.py)
    bunsan_install_python_module_compile(${module_path}/__init__.py)
endfunction()

function(bunsan_install_python_module_file)
    bunsan_install_python_module_common(FILE ${ARGN})

    install(FILES ${ARG_FILE} DESTINATION ${module_directory} RENAME ${module_name}.py)
    bunsan_install_python_module_compile(${module_path}.py)
endfunction()

function(bunsan_install_python_package)
    set(options)
    set(one_value_args PACKAGE DIRECTORY)
    set(multi_value_args)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})
    bunsan_required_arguments(ARG ${one_value_args})

    bunsan_get_install_python_package_components(
        PACKAGE ${ARG_PACKAGE}
        PATH path
    )
    install(DIRECTORY ${ARG_DIRECTORY}/ DESTINATION ${path})
    file(GLOB_RECURSE pyfiles
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_DIRECTORY}
        ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_DIRECTORY}/*.py
    )
    foreach(pyfile ${pyfiles})
        bunsan_install_python_module_compile(${path}/${pyfile})
    endforeach()
endfunction()
