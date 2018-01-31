find_program(Botan_EXECUTABLE botan)
if(Botan_EXECUTABLE)
    execute_process(COMMAND ${Botan_EXECUTABLE} config prefix
                    OUTPUT_VARIABLE Botan_PREFIX)
    separate_arguments(Botan_PREFIX)

    execute_process(COMMAND ${Botan_EXECUTABLE} config cflags
                    OUTPUT_VARIABLE Botan_CFLAGS)
    separate_arguments(Botan_CFLAGS)

    execute_process(COMMAND ${Botan_EXECUTABLE} config ldflags
                    OUTPUT_VARIABLE Botan_LDFLAGS)
    separate_arguments(Botan_LDFLAGS)

    execute_process(COMMAND ${Botan_EXECUTABLE} config libs
                    OUTPUT_VARIABLE Botan_LIBS)
    separate_arguments(Botan_LIBS)

    set(Botan_INCLUDE_DIRS)
    set(Botan_LIBRARIES)
    set(Botan_LIBNAME)
    set(Botan_LIBDIRS)

    foreach(arg ${Botan_CFLAGS})
        string(STRIP ${arg} arg)
        if(arg MATCHES ^-I)
            string(REGEX REPLACE "^-I" "" include_dir ${arg})
            list(APPEND Botan_INCLUDE_DIRS ${include_dir})
        endif()
    endforeach()

    foreach(arg ${Botan_LDFLAGS})
        string(STRIP ${arg} arg)
        if(arg MATCHES ^-L)
            string(REGEX REPLACE "^-L" "" library_dir ${arg})
            list(APPEND Botan_LIBDIRS ${library_dir})
        endif()
    endforeach()

    foreach(libarg ${Botan_LIBS})
        string(STRIP ${libarg} libarg)
        if(libarg MATCHES ^-l)
            string(REGEX REPLACE "^-l" "" libname ${libarg})
            if(libname MATCHES ^botan-)
                string(REGEX REPLACE "^botan-" "" Botan_VERSION ${libname})
                set(Botan_LIBNAME ${libname})
            else()
                list(APPEND Botan_LIBRARIES ${libname})
            endif()
        endif()
    endforeach()

    find_library(Botan_LOCATION ${Botan_LIBNAME} PATHS ${Botan_LIBDIRS})
    message(STATUS "Found Botan-${Botan_VERSION}")
    message(STATUS "  Location: ${Botan_LOCATION}")
    message(STATUS "  Includes: ${Botan_INCLUDE_DIRS}")
    message(STATUS "  Dependencies: ${Botan_LIBRARIES}")
    list(APPEND Botan_LIBRARIES ${Botan_LOCATION})
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Botan
    REQUIRED_VARS
        Botan_EXECUTABLE
        Botan_INCLUDE_DIRS
        Botan_LIBRARIES
    VERSION_VAR Botan_VERSION
)
