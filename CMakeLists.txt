cmake_minimum_required(VERSION 3.27)
cmake_policy(VERSION ${CMAKE_VERSION})

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

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

if (NOT ${CMAKE_GENERATOR} MATCHES "Ninja")
	message(WARNING "Consider configuring cmake with `-G Ninja` for better performance")
endif()

include(ccache)
#include(perf)

include(packages)

include(dependencies)

include(compiler)

include(linker)

add_subdirectory(src)
add_subdirectory(asset)
add_subdirectory(test)
add_subdirectory(bin)

include(convenience)

include(summary)

