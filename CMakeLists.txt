cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_bacs_common)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

bunsan_find_bunsan_package(bunsan_common bunsan_common)

bunsan_add_protobuf_cxx_library(
    TARGET ${PROJECT_NAME}
    PROTOS
        bacs/callback.proto
        bacs/file.proto
        bacs/process.proto
        bacs/utility.proto
    LIBRARIES ${bunsan_common_LIBRARIES}
    INCLUDE_DIRECTORIES ${bunsan_common_INCLUDE_DIRS}
    INSTALL
)

bunsan_install_headers()
bunsan_install_project()

bunsan_include_tests()
