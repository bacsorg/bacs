cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_common)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

if(UNIX)
    set(runtime_scope PUBLIC)
    set(runtime_libs ${CMAKE_DL_LIBS})
    set(runtime_srcs
        src/runtime/unix/stacktrace.cpp
    )
    include(CheckIncludeFileCXX)
    CHECK_INCLUDE_FILE_CXX(dlfcn.h BUNSAN_HAS_DLFCN)
    if(BUNSAN_HAS_DLFCN)
        list(APPEND runtime_srcs
             src/runtime/unix/stream-dlfcn.cpp)
    else()
        list(APPEND runtime_srcs
             src/runtime/unix/stream-fallback.cpp)
    endif()
elseif(WIN32)
    set(runtime_scope PRIVATE)
    set(runtime_srcs
        src/runtime/win32/stacktrace.cpp
    )
    if(MSVC)
        set(runtime_libs dbghelp)
        list(APPEND runtime_srcs
             src/runtime/win32/stream-msvc.cpp)
    else()
        set(runtime_libs psapi)
        list(APPEND runtime_srcs
             src/runtime/win32/stream-fallback.cpp)
    endif()
else()
    set(runtime_scope PRIVATE)
    set(runtime_srcs
        src/runtime/fallback/stacktrace.cpp
    )
endif()
bunsan_add_library(${PROJECT_NAME}
    src/tempfile.cpp
    src/error.cpp
    src/categorized_error.cpp
    src/system_error.cpp
    src/fnmatch.cpp

    src/application/application.cpp
    src/application/global_registry.cpp

    src/runtime/demangle.cpp
    src/runtime/detail/format.cpp
    ${runtime_srcs}

    src/filesystem/error.cpp
    src/filesystem/operations.cpp

    src/log/core.cpp
    src/log/trivial.cpp
)
target_link_libraries(${PROJECT_NAME} ${runtime_scope} ${runtime_libs})
bunsan_use_threads(${PROJECT_NAME})
bunsan_use_boost(${PROJECT_NAME}
    system
    filesystem
    log
    log_setup
    program_options
    regex
    serialization
    thread
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_project()

bunsan_include_tests()
