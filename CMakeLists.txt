cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bacs NONE)

add_subdirectory(bunsan)
add_subdirectory(bacs)
