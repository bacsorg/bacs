if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wpedantic -Wextra -Wno-multichar")
    if(UNIX)
        set(CMAKE_CXX_FLAGS "-rdynamic ${CMAKE_CXX_FLAGS}")
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")
endif()

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "-std=c11 -Wall -Wpedantic -Wextra")
    if(UNIX)
        set(CMAKE_C_FLAGS "-rdynamic ${CMAKE_C_FLAGS}")
    endif()
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_RELEASE "-O2")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)
    set(linker_flags "-Wl,--no-as-needed") # for bunsan::factory plugins
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${linker_flags}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${linker_flags}")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${linker_flags}")
endif()

# Note: defined as empty string by default.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug, Release, RelWithDebInfo and MinSizeRel." FORCE)
endif()

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

add_definitions(-DBOOST_ALL_NO_LIB)
if(BUILD_SHARED_LIBS)
    add_definitions(-DBOOST_ALL_DYN_LINK)
    set(Boost_USE_STATIC_LIBS OFF)
else()
    set(Boost_USE_STATIC_LIBS ON)
endif()
set(Boost_USE_MULTITHREADED ON)

set(ENABLE_TESTS ON CACHE BOOL "Do you want to enable testing?")
