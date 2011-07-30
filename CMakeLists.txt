project(bunsan_pm)

cmake_minimum_required(VERSION 2.8)

set(
	xmlrpc_link_lib
	xmlrpc++
	xmlrpc_client++
	xmlrpc_cpp
	xmlrpc_server++
	xmlrpc_server_abyss++
	xmlrpc_server_cgi++
	xmlrpc_server_pstream++
	xmlrpc_util
)

# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package (Boost COMPONENTS system filesystem serialization iostreams unit_test_framework program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
#set(Boost_LIBRARIES boost_system boost_filesystem boost_serialization boost_iostreams boost_unit_test_framework boost_program_options)
#include_directories(/usr/local/include)
#link_directories(/usr/local/lib)
message("We have to check pthread library on win* platform")
#find_package(Threads)
#target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)

add_definitions(-std=c++0x -pthread -g -D_GLIBCXX_USE_NANOSLEEP)

include_directories(include)

add_library(${PROJECT_NAME} SHARED src/lib/repository.cpp src/lib/repository_native.cpp src/lib/entry.cpp src/lib/hash.cpp)
target_link_libraries(${PROJECT_NAME} bunsan_common bunsan_compatibility ${Boost_LIBRARIES})

add_executable(bunsan_repo src/main.cpp)
target_link_libraries(bunsan_repo ${PROJECT_NAME})

add_library(bunsan_pm_c SHARED src/lib/bunsan_pm_c.cpp)
target_link_libraries(bunsan_pm_c bunsan_pm)

# install

install(DIRECTORY include DESTINATION .)
install(TARGETS ${PROJECT_NAME} bunsan_pm_c DESTINATION lib)
install(TARGETS bunsan_repo DESTINATION bin)

# testing

enable_testing()
add_subdirectory(tests ${PROJECT_BINARY_DIR}/tests)

