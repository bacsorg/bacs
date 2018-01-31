if(EXISTS /etc/arch-release)
    set(CMAKE_INSTALL_SBINDIR bin CACHE PATH "system admin executables (bin)")
    set(CMAKE_INSTALL_LIBEXECDIR lib/${PROJECT_NAME}
        CACHE PATH "program executables (lib/${PROJECT_NAME})")
    set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "object code libraries (lib)")
    mark_as_advanced(
        CMAKE_INSTALL_SBINDIR
        CMAKE_INSTALL_LIBEXECDIR
        CMAKE_INSTALL_LIBDIR)
endif()
include(GNUInstallDirs)

set(CMAKE_INSTALL_RPATH
    "$ORIGIN/../${CMAKE_INSTALL_LIBDIR};${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" CACHE STRING
    "A semicolon-separated list specifying the rpath to use in installed targets (for platforms that support it).")
mark_as_advanced(CMAKE_INSTALL_RPATH)

set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY YES CACHE BOOL "Do not use package registry (YES)")

function(bunsan_get_package_install_path CONFIG NAME)
    set(${CONFIG} ${CMAKE_INSTALL_LIBDIR}/${NAME} PARENT_SCOPE)
endfunction()
