project(repo)

cmake_minimum_required(VERSION 2.8)

if (NOT DEFINED BUNSAN_COMMON_DIR)
	message(SEND_ERROR "you have to define BUNSAN_COMMON_DIR variable")
endif()

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
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)
find_package (Boost COMPONENTS system filesystem serialization iostreams unit_test_framework program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
#set(Boost_LIBRARIES boost_system boost_filesystem boost_serialization boost_iostreams boost_unit_test_framework boost_program_options)
#include_directories(/usr/local/include)
#link_directories(/usr/local/lib)
message("We have to check pthread library on win* platform")
#find_package (Threads)
#target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)

add_definitions(-std=c++0x -pthread -g)

add_subdirectory(${BUNSAN_COMMON_DIR} ${PROJECT_BINARY_DIR}/common)

include_directories(. ${BUNSAN_COMMON_DIR})

if (UNIX)
	set(impl_dir impl/unix)
elseif(WIN32)
	set(impl_dir impl/win32)
elseif(APPLE)
	set(impl_dir impl/apple)
else(SEND_ERROR)
	message( "Unknown platform")
endif()

add_library(its_pkg SHARED repository.cpp repository_native.cpp executor.cpp tempfile.cpp ${impl_dir}/execute.cpp)

add_executable(${PROJECT_NAME} main.cpp)
#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${xmlrpc_link_lib})
target_link_libraries(${PROJECT_NAME} its_pkg ${Boost_LIBRARIES} ${xmlrpc_link_lib})

