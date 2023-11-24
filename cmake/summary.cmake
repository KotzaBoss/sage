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
message(STATUS "")
message(STATUS "Custom Details: ..................................................")
message(STATUS "")
cmake_print_variables(CCACHE_ENABLED CCACHE)
cmake_print_variables(SAGE_VERBOSE)
message(STATUS "")
message(STATUS "Show usage: .............................................")
message(STATUS "")
message(STATUS "	cmake --build build -- --quiet usage")
message(STATUS "")
message(STATUS "▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄")
