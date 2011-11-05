project(bunsan_pm)

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_SOURCE_DIR}/user-config.cmake OPTIONAL)

# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem serialization iostreams unit_test_framework program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if (${CMAKE_COMPILER_IS_GNUCC})
	set(gcc_flags -g -Wall -Wextra -Wfatal-errors)
	if (${MINGW})
		# MinGW has no pthread support
		add_definitions(-std=c++0x ${gcc_flags})
	else()
		add_definitions(-std=c++0x -pthread ${gcc_flags})
	endif()
endif()

include_directories(include)

aux_source_directory(src/lib lib_srcs)
add_library(${PROJECT_NAME} SHARED ${lib_srcs})
target_link_libraries(${PROJECT_NAME} bunsan_common cryptopp ${Boost_LIBRARIES})

add_executable(bunsan_repo src/main.cpp)
target_link_libraries(bunsan_repo ${PROJECT_NAME})

add_library(bunsan_pm_c SHARED src/bunsan_pm_c.cpp)
target_link_libraries(bunsan_pm_c bunsan_pm)

# install

install(DIRECTORY include DESTINATION .)
install(TARGETS ${PROJECT_NAME} bunsan_pm_c DESTINATION lib)
install(TARGETS bunsan_repo DESTINATION bin)

# testing

enable_testing()
add_subdirectory(tests ${PROJECT_BINARY_DIR}/tests)

