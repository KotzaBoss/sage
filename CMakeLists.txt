cmake_minimum_required(VERSION 3.27)
cmake_policy(VERSION ${CMAKE_VERSION})

project(SAGE
	VERSION 0.0.1
	DESCRIPTION "Super Advanced Game Engine"
	LANGUAGES C CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

message("==================================================================")

include(assert)
include(options)
include(section)
include(docs)

sage_options(ADD VERBOSE DOC TRIVIAL INIT OFF)
if (SAGE_OPT_VERBOSE)
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

include(compiler)

include(linker)

include(packages)

include(dependencies)

add_subdirectory(src)
add_subdirectory(asset)
add_subdirectory(perf)
add_subdirectory(bin)
add_subdirectory(docs)

add_subdirectory(test)

include(convenience)

include(summary)

