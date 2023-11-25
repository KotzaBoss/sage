section_start("ccache")

set(CCACHE_WEBSITE "https://ccache.dev/")

sage_options(ADD CCACHE_ENABLED DOC "Use [`ccache`](${CCACHE_WEBSITE}) to speedup subsequent compilations" INIT OFF)
if (SAGE_OPT_CCACHE_ENABLED)
	if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		message(WARNING "Consider using ccache only for Debug builds")
	endif()

	find_program(CCACHE ccache)
	if (CCACHE)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
		section_pass("${CCACHE}")
	else()
		set(SAGE_OPT_CCACHE_ENABLED OFF)
		message(WARNING "Unable to find `ccache` program, you may acquire it through ${CCACHE_WEBSITE} or your system's package manager")
		section_fail("${CCACHE}")
	endif()
else()
	message(WARNING "Consider configuring cmake with `-DSAGE_OPT_CCACHE_ENABLED=ON`")
	section_pass("Skip")
endif()

