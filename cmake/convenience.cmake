add_custom_target(todo
		# Keep this directory so that grep displays relative paths
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		COMMENT "Write build/todo files"
		COMMAND mkdir -p ${CMAKE_BINARY_DIR}/todo
		COMMAND grep -nrE --color=always -C10 "\"(TODO|FIXME):.*\"" src sandbox test > ${CMAKE_BINARY_DIR}/todo/long
		COMMAND grep  -nrE --only-matching --color=never "\"(TODO|FIXME):.*\"" src sandbox test
				| sed -e "s/:/█/" -e "s/:/█/"
				| column --separator="█" -t
				| tee ${CMAKE_BINARY_DIR}/todo/short
		VERBATIM
	)

docs(GET sage_docs TARGET sage)
docs(GET perf_docs TARGET perf)
docs(SET GLOBAL DOCS
		"Show usage:"
		"    cmake --build build -- usage"
		"Build todos:"
		"    cmake --build build -- todo"
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

