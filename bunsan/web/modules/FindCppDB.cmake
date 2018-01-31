find_package(Booster REQUIRED)

find_path(CPPDB_INCLUDE_DIR cppdb/defs.h)
find_library(CPPDB_LIBRARY cppdb)

if(CPPDB_INCLUDE_DIR AND CPPDB_LIBRARY)
    set(CppDB_INCLUDE_DIRS ${CPPDB_INCLUDE_DIR} ${Booster_INCLUDE_DIRS})
    set(CppDB_LIBRARIES ${CPPDB_LIBRARY} ${Booster_LIBRARIES})
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CppDB
    REQUIRED_VARS
        CppDB_INCLUDE_DIRS
        CppDB_LIBRARIES
)
