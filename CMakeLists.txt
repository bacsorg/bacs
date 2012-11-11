project(bunsan_utility)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_bunsan(common)

bunsan_use_boost(system filesystem serialization unit_test_framework thread)

bunsan_add_shared_library(${PROJECT_NAME}
    src/error.cpp
    src/utility.cpp
    src/executor.cpp
    src/resolver.cpp

    src/builder.cpp
    src/builders/cmake.hpp
    src/builders/cmake.cpp
    src/builders/conf_make_install.hpp
    src/builders/conf_make_install.cpp

    src/fetcher.cpp
    src/fetchers/curl.hpp
    src/fetchers/wget.cpp
    src/fetchers/wget.hpp
    src/fetchers/curl.cpp
    src/fetchers/copy.hpp
    src/fetchers/copy.cpp

    src/archiver.cpp
    src/archivers/tar.hpp
    src/archivers/cwd_split.cpp
    src/archivers/7z.hpp
    src/archivers/tar.cpp
    src/archivers/7z.cpp
    src/archivers/cwd_split.hpp

    src/maker.cpp
    src/makers/make.cpp
    src/makers/make.hpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})

bunsan_include_tests()
