project(bunsan_pm)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_bunsan(common process utility)
bunsan_use(cryptopp)
bunsan_use_boost(system filesystem serialization program_options thread)

bunsan_add_shared_library(${PROJECT_NAME}
    src/lib/error.cpp
    src/lib/index.cpp
    src/lib/entry.cpp
    src/lib/checksum.cpp
    src/lib/repository_native_create.cpp
    src/lib/repository_native.cpp
    src/lib/repository_native.hpp
    src/lib/repository.cpp
    src/lib/repository_native_helpers.cpp

    src/lib/cache.cpp
)
bunsan_use(${PROJECT_NAME})

bunsan_add_shared_library(${PROJECT_NAME}_compatibility
    src/compatibility/repository_cpp.cpp
    src/compatibility/repository_c.cpp
)

bunsan_add_executable(${PROJECT_NAME}_cli
    src/bin/cli.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME} ${PROJECT_NAME}_compatibility ${PROJECT_NAME}_cli)

bunsan_include_tests()
