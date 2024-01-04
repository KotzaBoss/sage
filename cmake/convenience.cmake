docs(GET sage_docs TARGET sage PREPEND "    ")
docs(GET perf_docs TARGET perf PREPEND "    ")
docs(SET GLOBAL DOCS
		"Show usage:"
		"    cmake --build build -- usage"
		"Build all:"
		"    cmake --build build"
		"Run tests:"
		"    ctest --test-dir ${CMAKE_BINARY_DIR}/test"
		"Run sage:"
		"${sage_docs}"
		"Perf sage:"
		"${perf_docs}"
	)

docs(GET usage GLOBAL ESCAPE_NEWLINE)
add_custom_target(usage
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMAND echo -e "${usage}"
		VERBATIM
	)

