project(bunsan_bacs_archive)

cmake_minimum_required(VERSION 2.8)

if(${CMAKE_COMPILER_IS_GNUCXX})
    set(gxx_flags "-std=c++0x -Wall -Wextra")
    if(${UNIX})
        set(gxx_flags "-pthread ${gxx_flags}")
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG "-g ${gxx_flags}")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 ${gxx_flags}")
endif()

include(${CMAKE_SOURCE_DIR}/user-config.cmake OPTIONAL)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif()

if(NOT DEFINED ENABLE_TESTS)
    set(ENABLE_TESTS ON CACHE BOOL "Do you want to enable testing?" FORCE)
endif()

# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem serialization iostreams unit_test_framework program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(include)

aux_source_directory(src/lib lib_srcs)
add_library(${PROJECT_NAME} SHARED ${lib_srcs})
target_link_libraries(${PROJECT_NAME} bunsan_common bunsan_utility cryptopp ${Boost_LIBRARIES})

add_executable(${PROJECT_NAME}_server src/main.cpp)
target_link_libraries(${PROJECT_NAME}_server ${PROJECT_NAME})

aux_source_directory(src/compatibility compatibility_srcs)
add_library(${PROJECT_NAME}_compatibility SHARED ${compatibility_srcs})
target_link_libraries(${PROJECT_NAME}_compatibility ${PROJECT_NAME})

# install

install(DIRECTORY include DESTINATION .)
install(TARGETS
    ${PROJECT_NAME}
    ${PROJECT_NAME}_repo
    ${PROJECT_NAME}_compatibility
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

# testing

if(${ENABLE_TESTS})
    enable_testing()
    add_subdirectory(tests ${PROJECT_BINARY_DIR}/tests)
endif()

