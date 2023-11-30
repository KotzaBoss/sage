# Make sure its the last line in the top level CMakeLists.txt

# Force inclusion of print helpers
include(CMakePrintHelpers)

sage_options(GET ALL OUT options DOC docs)
foreach(opt_doc IN ZIP_LISTS options docs)
	add_feature_info(${opt_doc_0} ${opt_doc_0} "${opt_doc_1}")
endforeach()

set(variables
		CMAKE_MODULE_PATH
		CMAKE_GENERATOR
		CMAKE_BUILD_TYPE
		CMAKE_CXX_COMPILER CMAKE_CXX_COMPILER_ID CMAKE_CXX_COMPILER_VERSION
		CMAKE_CXX_STANDARD CMAKE_CXX_STANDARD_REQUIRED
		CMAKE_CXX_EXTENSIONS
		CXX_COMPILER_LAUNCHER C_COMPILER_LAUNCHER
	)
foreach(var ${variables})
	add_feature_info(${var} ${var} "${${var}}")
endforeach()

if (SAGE_OPT_VERBOSE)
	# Project wide properties
	set(properties
		COMPILE_OPTIONS COMPILE_DEFINITIONS
		LINK_OPTIONS
		INCLUDE_DIRECTORIES
		LINK_DIRECTORIES
		SUBDIRECTORIES
		)
	foreach (prop ${properties})
		block()
			get_directory_property(items DIRECTORY ${CMAKE_SOURCE_DIR} ${prop})
			list(JOIN items " " description)
			add_feature_info(${prop} items "${description}")
		endblock()
	endforeach()

	get_directory_property(tests DIRECTORY ${PROJECT_SOURCE_DIR}/test TESTS)
	list(JOIN tests " " description)
	add_feature_info(TESTS tests "${description}")
endif()

message("")
message(STATUS "▛▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀")
message(STATUS "	${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "	${PROJECT_DESCRIPTION}")
message("")
message(STATUS "	When in doubt... cmake --build build -- --quiet usage")
message("")
message(STATUS "..................................................................")
message("")
feature_summary(
		WHAT
			ENABLED_FEATURES
			DISABLED_FEATURES
	)
message(STATUS "▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄")
