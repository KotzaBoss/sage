section_start("Perf")

# TODO

include(FetchContent)
include(ExternalProject)

find_program(PERF perf)
if (NOT PERF)
	section_fail("${PERF}")
else()
	ExternalProject_Add(
			flamegraph
			GIT_REPOSITORY https://github.com/brendangregg/FlameGraph
			GIT_TAG origin/master
			CONFIGURE_COMMAND ""
			INSTALL_COMMAND ""
			BUILD_COMMAND ""
			UPDATE_COMMAND ""
			BUILD_ALWAYS OFF
		)
	section_pass("${PERF}")
endif()

