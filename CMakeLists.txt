project(bunsan_utility)

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_SOURCE_DIR}/user-config.cmake OPTIONAL)

# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem serialization iostreams unit_test_framework program_options REQUIRED)
list(APPEND includes ${Boost_INCLUDE_DIRS})
list(APPEND libraries ${Boost_LIBRARIES})

list(APPEND libraries bunsan_common)

if(${CMAKE_COMPILER_IS_GNUCC})
	set(gcc_flags -g -std=c++0x -Wall -Wextra)
	if(${MINGW})
		# MinGW has no pthread support
		add_definitions(${gcc_flags})
	else()
		add_definitions(-pthread ${gcc_flags})
	endif()
endif()

aux_source_directory(src srcs)

aux_source_directory(src/archivers archivers_srcs)
list(APPEND srcs ${archivers_srcs})

aux_source_directory(src/builders builders_srcs)
list(APPEND srcs ${builders_srcs})

aux_source_directory(src/fetchers fetchers_srcs)
list(APPEND srcs ${fetchers_srcs})

include_directories(include ${includes})

add_library(${PROJECT_NAME} SHARED ${srcs})
target_link_libraries(${PROJECT_NAME} ${libraries})

# install

install(DIRECTORY include DESTINATION .)
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

# testing

enable_testing()
add_subdirectory(tests ${PROJECT_BINARY_DIR}/tests)

