project(bunsan_pm)

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_SOURCE_DIR}/user-config.cmake OPTIONAL)

# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem serialization iostreams unit_test_framework program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if(${CMAKE_COMPILER_IS_GNUCC})
	set(gcc_flags -g -std=c++0x -Wall -Wextra)
	if(${MINGW})
		# MinGW has no pthread support
		add_definitions(${gcc_flags})
	else()
		add_definitions(-pthread ${gcc_flags})
	endif()
endif()

include_directories(include)

aux_source_directory(src/lib lib_srcs)
add_library(${PROJECT_NAME} SHARED ${lib_srcs})
target_link_libraries(${PROJECT_NAME} bunsan_common cryptopp ${Boost_LIBRARIES})

add_executable(${PROJECT_NAME}_repo src/main.cpp)
target_link_libraries(${PROJECT_NAME}_repo ${PROJECT_NAME})

add_library(${PROJECT_NAME}_c SHARED src/compatibility/bunsan_pm_c.cpp)
target_link_libraries(${PROJECT_NAME}_c ${PROJECT_NAME})

add_library(${PROJECT_NAME}_compatibility SHARED src/compatibility/repository.cpp)
target_link_libraries(${PROJECT_NAME}_compatibility ${PROJECT_NAME})

# install

install(DIRECTORY include DESTINATION .)
install(TARGETS
	${PROJECT_NAME}
	${PROJECT_NAME}_c
	${PROJECT_NAME}_repo
	${PROJECT_NAME}_compatibility
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

# testing

enable_testing()
add_subdirectory(tests ${PROJECT_BINARY_DIR}/tests)

