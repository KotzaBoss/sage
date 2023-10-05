add_custom_target(todo
		ALL
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

add_custom_target(usage
		ALL
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMAND echo "Show usage:     cmake --build build -- --quiet usage"
		COMMAND echo "Build all:      cmake --build build"
		COMMAND echo "Run sandbox:    cmake --build build -- sandbox && ./build/sandbox/sandbox"
		COMMAND echo "Build todos:    cmake --build build -- --quiet todo"
		VERBATIM
	)

