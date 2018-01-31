find_path(BOOSTER_INCLUDE_DIR booster/config.h)
find_library(BOOSTER_LIBRARY booster)

if(BOOSTER_INCLUDE_DIR AND BOOSTER_LIBRARY)
    set(Booster_INCLUDE_DIRS ${BOOSTER_INCLUDE_DIR})
    set(Booster_LIBRARIES ${BOOSTER_LIBRARY})
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Booster
    REQUIRED_VARS
        Booster_INCLUDE_DIRS
        Booster_LIBRARIES
)
