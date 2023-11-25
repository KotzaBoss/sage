# Force inclusion of print helpers
include(CMakePrintHelpers)

message("")
message(STATUS "▛▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀")
message(STATUS "	${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "	${PROJECT_DESCRIPTION}")
message(STATUS "")
message(STATUS "CMake Details: ...................................................")
message(STATUS "")
cmake_print_variables(CMAKE_MODULE_PATH)
message(STATUS "")
cmake_print_variables(CMAKE_GENERATOR)
cmake_print_variables(CMAKE_BUILD_TYPE)
message(STATUS "")
cmake_print_variables(CMAKE_CXX_COMPILER)
cmake_print_variables(CMAKE_CXX_COMPILER_ID)
cmake_print_variables(CMAKE_CXX_COMPILER_VERSION)
cmake_print_variables(CMAKE_CXX_STANDARD)
cmake_print_variables(CMAKE_CXX_STANDARD_REQUIRED)
cmake_print_variables(CMAKE_CXX_EXTENSIONS)
message(STATUS "")
cmake_print_variables(CMAKE_LINKER)
if (SAGE_OPT_VERBOSE)
cmake_print_properties(DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
		PROPERTIES
			COMPILE_OPTIONS
			COMPILE_DEFINITIONS
			RULE_LAUNCH_COMPILE
			LINK_OPTIONS
			INCLUDE_DIRECTORIES
			LINK_DIRECTORIES
	)
cmake_print_properties(DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
		PROPERTIES
			SUBDIRECTORIES
	)
endif()
message(STATUS "")
message(STATUS "Custom Details: ..................................................")
message(STATUS "")
sage_options(GET ALL OUT sage_vars)
foreach (v ${sage_vars})
	if (v MATCHES ".*CCACHE.*")
		cmake_print_variables(${v} CCACHE)
	else()
		cmake_print_variables(${v})
	endif()
endforeach()
message(STATUS "")
message(STATUS "Show usage: .............................................")
message(STATUS "")
message(STATUS "	cmake --build build -- --quiet usage")
message(STATUS "")
message(STATUS "▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄")

