project(bunsan_pm)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

include_directories(include)

# bunsan: common utility
list(APPEND libraries bunsan_common bunsan_utility)

# Crypto++
list(APPEND libraries cryptopp)

bunsan_use_boost(system filesystem serialization unit_test_framework program_options thread)

add_library(${PROJECT_NAME} SHARED
    src/lib/error.cpp
    src/lib/depends.cpp
    src/lib/entry.cpp
    src/lib/checksum.cpp
    src/lib/repository_native_create.cpp
    src/lib/repository_native.cpp
    src/lib/repository_native.hpp
    src/lib/repository.cpp
    src/lib/repository_native_helpers.cpp
)
target_link_libraries(${PROJECT_NAME} ${libraries})

add_library(${PROJECT_NAME}_compatibility SHARED
    src/compatibility/repository_cpp.cpp
    src/compatibility/repository_c.cpp
)
target_link_libraries(${PROJECT_NAME}_compatibility ${PROJECT_NAME} ${libraries})

add_executable(${PROJECT_NAME}_cli
    src/bin/cli.cpp
)
target_link_libraries(${PROJECT_NAME}_cli ${PROJECT_NAME} ${libraries})

install(DIRECTORY include DESTINATION .)
bunsan_install_targets(${PROJECT_NAME} ${PROJECT_NAME}_compatibility ${PROJECT_NAME}_cli)

bunsan_include_tests()
