function(bunsan_pm_cpack)
	set(CPACK_GENERATOR "TGZ")
	set(CPACK_PACKAGE_VERSION_MAJOR 0)
	set(CPACK_PACKAGE_VERSION_MINOR 0)
	set(CPACK_PACKAGE_VERSION_PATCH 0)
	set(CPACK_PACKAGE_FILE_NAME "pkg")
	include(CPack)
endfunction()

