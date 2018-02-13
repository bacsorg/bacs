cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bacs_problem)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

bunsan_find_bunsan_package(bacs_common bacs_common)
bunsan_add_protobuf_cxx_library(
    TARGET ${PROJECT_NAME}_pb
    PROTOS
        bacs/problem/problem.proto
        bacs/problem/result.proto

        bacs/problem/single/testing.proto
        bacs/problem/single/process/file.proto
        bacs/problem/single/process/settings.proto
        bacs/problem/single/process/execution.proto
        bacs/problem/single/task.proto
        bacs/problem/single/problem.proto
        bacs/problem/single/intermediate.proto
        bacs/problem/single/result.proto
    LIBRARIES ${bacs_common_LIBRARIES}
    INCLUDE_DIRECTORIES ${bacs_common_INCLUDE_DIRS}
    INSTALL
)

bunsan_add_library(${PROJECT_NAME}
    src/buildable.cpp
    src/utility.cpp
    src/id.cpp
    src/importer.cpp
    src/statement.cpp
    src/resource/parse.cpp
    src/system_verifier.cpp

    src/utilities/alias.cpp
    src/utilities/none.cpp
    src/utilities/single.cpp
    src/utilities/dot_net_solution.cpp

    src/statement_versions/copy.cpp
    src/statement_versions/pdflatex.cpp

    src/single/driver.cpp
    src/single/generator.cpp

    src/single/test/list_storage.cpp
    src/single/test/matcher.cpp
    src/single/test/storage.cpp

    src/single/drivers/simple0/driver.cpp
    src/single/drivers/simple0/tests.cpp

    src/single/drivers/polygon_codeforces_com/driver.cpp
    src/single/drivers/polygon_codeforces_com/statement.cpp
    src/single/drivers/polygon_codeforces_com/tests.cpp

    src/single/generators/internal0.cpp

    src/single/importer.cpp
)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_pm bunsan_pm)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_protobuf bunsan_protobuf)
bunsan_use_target(${PROJECT_NAME} ${PROJECT_NAME}_pb)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_project()

bunsan_include_tests()
