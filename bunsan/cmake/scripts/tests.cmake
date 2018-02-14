macro(bunsan_enable_testing)
    if(ENABLE_TESTS)
        enable_testing()
    endif()
endmacro()

macro(bunsan_include_tests)
    if(ENABLE_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()
endmacro()

# Note should not be emptied in bunsan_tests_project_header
# to allow user add dependencies before tests project.
set(BUNSAN_TESTS_DEFINITIONS)
set(BUNSAN_TESTS_INCLUDE_DIRS)
set(BUNSAN_TESTS_LIBRARIES)

macro(bunsan_tests_compile_definitions)
    list(APPEND BUNSAN_TESTS_DEFINITIONS ${ARGN})
endmacro()

macro(bunsan_tests_include_directories)
    list(APPEND BUNSAN_TESTS_INCLUDE_DIRS ${ARGN})
endmacro()

macro(bunsan_tests_link_libraries)
    list(APPEND BUNSAN_TESTS_LIBRARIES ${ARGN})
endmacro()

macro(bunsan_tests_use_package package)
    bunsan_find_package(${package} ${ARGN})
    bunsan_tests_include_directories(${${package}_INCLUDE_DIRS})
    bunsan_tests_link_libraries(${${package}_LIBRARIES})
endmacro()

macro(bunsan_tests_use_bunsan_package package)
    bunsan_find_bunsan_package(${package} ${ARGN})
    bunsan_tests_link_libraries(${${package}_LIBRARIES})
endmacro()

macro(bunsan_tests_use_parent_project)
    bunsan_tests_link_libraries(${PROJECT_NAME})
endmacro()

# exports empty ${bunsan_tests_targets},
# ${bunsan_tests_sources} and ${BUNSAN_TESTS}
macro(bunsan_tests_project_header)
    message(STATUS "Tests were enabled")
    get_filename_component(BUNSAN_TESTS_PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
    get_filename_component(BUNSAN_TESTS_PARENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY)
    set(BUNSAN_TESTS_ENV
        "BUNSAN_SOURCE_DIR=${BUNSAN_TESTS_PARENT_SOURCE_DIR}"
        "BUNSAN_BINARY_DIR=${BUNSAN_TESTS_PARENT_BINARY_DIR}"
    )
    set(BUNSAN_TESTS)
    set(BUNSAN_TESTS_SOURCES)
    set(BUNSAN_TESTS_TARGETS)

    # default settings
    bunsan_find_package(Boost unit_test_framework)
    bunsan_tests_include_directories(${Boost_INCLUDE_DIRS})
    bunsan_tests_link_libraries(${Boost_LIBRARIES})
endmacro()

# updates ${BUNSAN_TESTS_TARGETS},
# ${BUNSAN_TESTS_SOURCES} and ${BUNSAN_TESTS}
macro(bunsan_tests_project_add_executable target)
    bunsan_add_executable(${target} ${ARGN})
    target_compile_definitions(${target} PRIVATE ${BUNSAN_TESTS_DEFINITIONS})
    target_include_directories(${target} PRIVATE ${BUNSAN_TESTS_INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ${BUNSAN_TESTS_LIBRARIES})
    bunsan_targets_finish_setup(${target})

    list(APPEND BUNSAN_TESTS_SOURCES ${ARGN})
    list(APPEND BUNSAN_TESTS_TARGETS ${target})
endmacro()

# updates ${BUNSAN_TESTS}
macro(bunsan_tests_project_add_custom_test test)
    add_test(${test} ${ARGN})
    set_tests_properties(${test} PROPERTIES ENVIRONMENT "${BUNSAN_TESTS_ENV}")
    list(APPEND BUNSAN_TESTS ${test})
endmacro()

# updates ${BUNSAN_TESTS_TARGETS},
# ${BUNSAN_TESTS_SOURCES} and ${BUNSAN_TESTS}
macro(bunsan_tests_project_add_test target)
    bunsan_tests_project_add_executable(${PROJECT_NAME}_test_${target} ${ARGN})
    set_target_properties(${PROJECT_NAME}_test_${target} PROPERTIES OUTPUT_NAME ${target})
    bunsan_tests_project_add_custom_test(${target} ${target} ${ARGN})
endmacro()

# updates ${BUNSAN_TESTS_TARGETS} and empty ${BUNSAN_TESTS_SOURCES}
macro(bunsan_tests_project_aux_add_tests)
    aux_source_directory(. bunsan_tests_aux_sources)

    foreach(source ${bunsan_tests_aux_sources})
        get_filename_component(target ${source} NAME_WE)
        bunsan_tests_project_add_test(${target} ${source})
    endforeach()
endmacro()

macro(bunsan_tests_project_footer)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources)
        add_subdirectory(resources)
    endif()
endmacro()

# exports ${BUNSAN_TESTS_SOURCES} and ${BUNSAN_TESTS_TARGETS}
macro(bunsan_tests_project)
    bunsan_tests_project_header()
    bunsan_tests_project_aux_add_tests()
    bunsan_tests_project_footer()
endmacro()
