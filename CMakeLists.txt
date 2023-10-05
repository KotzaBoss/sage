cmake_minimum_required(VERSION 3.26)

project(SAGE
	VERSION 0.0.1
	DESCRIPTION "Super Advanced Game Engine"
	LANGUAGES C CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

message("==================================================================")

include(section)

option(SAGE_VERBOSE OFF)
if (SAGE_VERBOSE)
	include(CMakePrintHelpers)
else()
	# Make the CMakePrintHelpers noop
	function(cmake_print_variables)
	endfunction()
	function(cmake_print_properties)
	endfunction()
endif()

include(build_type)

include(generator)

include(ccache)
#include(perf)

include(packages)

include(dependencies)

include(compiler)

include(linker)

include(subdirectories)

include(convenience)

include(summary)

