cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_bacs_problem)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

bunsan_find_bunsan_package(bunsan_bacs_common bunsan_bacs_common)
bunsan_add_protobuf_cxx_library(
    TARGET ${PROJECT_NAME}_pb
    PROTOS
        bacs/problem/problem.proto
        bacs/problem/result.proto
    LIBRARIES ${bunsan_bacs_common_LIBRARIES}
    INCLUDE_DIRECTORIES ${bunsan_bacs_common_INCLUDE_DIRS}
    INSTALL
)

bunsan_add_library(${PROJECT_NAME}
    src/buildable.cpp
    src/utility.cpp
    src/id.cpp
    src/importer.cpp
    src/statement.cpp
    src/resource/parse.cpp

    src/utilities/none.cpp
    src/utilities/single.cpp
    src/utilities/dot_net_solution.cpp

    src/statement_versions/copy.cpp
    src/statement_versions/pdflatex.cpp
)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_pm bunsan_pm)
bunsan_use_target(${PROJECT_NAME} ${PROJECT_NAME}_pb)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_project()

bunsan_include_tests()
