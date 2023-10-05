section_start("ccache")

option(CCACHE_ENABLED "Use ccache to speedup subsequent compilations" OFF)
if (NOT CCACHE_ENABLED)
	message(WARNING "Consider configuring cmake with `-DCCACHE_ENABLED=ON`")
	section_pass("Skip")
	return()
endif()

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	message(WARNING "Consider not using ccache for builds other than Debug")
endif()

find_program(CCACHE ccache)
if (CCACHE)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
	section_pass("${CCACHE}")
else()
	set(CCACHE_ENABLED OFF)
	section_fail("${CCACHE}")
endif()

