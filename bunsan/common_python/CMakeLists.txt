cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_common_python)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

bunsan_find_python3()
bunsan_install_python_package(
    PACKAGE bunsan
    DIRECTORY src
)
#bunsan_install_project()

#bunsan_include_tests()
