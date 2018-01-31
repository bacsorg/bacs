include(${CMAKE_CURRENT_LIST_DIR}/compiler.cmake)

# Import these packages to process dependencies correctly.
if(NOT DEFINED BUNSAN_PACKAGE_REGISTRY)
    set(BUNSAN_PACKAGE_REGISTRY)
endif()

macro(bunsan_find_package package)
    find_package(${package} REQUIRED ${ARGN})
    string(TOUPPER ${package} _package_upper)
    set(_package_include_dirs
            ${${package}_INCLUDE_DIRS}
            ${${_package_upper}_INCLUDE_DIRS}
    )
    set(_package_libraries
        ${${package}_LIBRARIES}
        ${${_package_upper}_LIBRARIES}
    )
    set(${package}_INCLUDE_DIRS ${_package_include_dirs})
    set(${package}_LIBRARIES ${_package_libraries})
endmacro()

macro(bunsan_find_bunsan_package package)
    if(DEFINED ${package}_SOURCE_DIR)
        set(${package}_FOUND TRUE)
        foreach(target ${ARGN})
            if(NOT TARGET ${target})
                set(${package}_FOUND FALSE)
                message(SEND_ERROR "Target ${target} is not defined for package ${package}!")
            endif()
        endforeach()
        set(${package}_INCLUDE_DIRS)
        set(${package}_LIBRARIES ${ARGN})
    else()
        find_package(${package} CONFIG REQUIRED COMPONENTS ${ARGN})
    endif()
    list(APPEND BUNSAN_PACKAGE_REGISTRY ${package})
endmacro()

# \param ARGN other dependencies
function(bunsan_add_imported_library target type prefix)
    message(STATUS "Added ${type} library ${target}")
    message(STATUS "  Includes: ${${prefix}_INCLUDE_DIRS}")
    add_library(${target} ${type} IMPORTED GLOBAL)
    list(GET ${prefix}_LIBRARIES 0 location)
    get_filename_component(soname ${location} NAME)
    set(deps ${${prefix}_LIBRARIES} ${ARGN})
    list(REMOVE_AT deps 0)
    message(STATUS "  Dependencies: ${deps}")
    message(STATUS "  Location: ${location}")
    message(STATUS "  Soname: ${soname}")
    set_target_properties(${target} PROPERTIES
        IMPORTED_LOCATION "${location}"
        IMPORTED_SONAME "${soname}"
        INTERFACE_INCLUDE_DIRECTORIES "${${prefix}_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${deps}")
endfunction()

macro(bunsan_use_target target package)
    if(NOT TARGET ${package})
        message(SEND_ERROR "${package} is not a target!")
    endif()
    target_link_libraries(${target} PUBLIC ${package})
endmacro()

macro(bunsan_use_package target package)
    bunsan_find_package(${package} ${ARGN})
    target_include_directories(${target} PUBLIC ${${package}_INCLUDE_DIRS})
    target_link_libraries(${target} PUBLIC ${${package}_LIBRARIES})
endmacro()

macro(bunsan_use_threads target)
    find_package(Threads REQUIRED)
    target_link_libraries(${target} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endmacro()

macro(bunsan_use_boost target)
    bunsan_use_package(${target} Boost COMPONENTS ${ARGN})
endmacro()

macro(bunsan_use_python target)
    bunsan_find_python()
    target_include_directories(${target} PUBLIC ${PYTHON_INCLUDE_DIRS})
    target_link_libraries(${target} PUBLIC ${PYTHON_LIBRARIES})
endmacro()

macro(bunsan_use_python2 target)
    bunsan_find_python2()
    target_include_directories(${target} PUBLIC ${PYTHON_INCLUDE_DIRS})
    target_link_libraries(${target} PUBLIC ${PYTHON_LIBRARIES})
endmacro()

macro(bunsan_use_python3 target)
    bunsan_find_python3()
    target_include_directories(${target} PUBLIC ${PYTHON_INCLUDE_DIRS})
    target_link_libraries(${target} PUBLIC ${PYTHON_LIBRARIES})
endmacro()

macro(bunsan_use_bunsan_package target package)
    bunsan_find_bunsan_package(${package} ${ARGN})
    target_include_directories(${target} PUBLIC ${${package}_INCLUDE_DIRS})
    target_link_libraries(${target} PUBLIC ${${package}_LIBRARIES})
endmacro()
