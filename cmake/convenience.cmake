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

add_custom_target(usage
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMAND echo "Show usage:"
		COMMAND echo "    cmake --build build -- usage"
		COMMAND echo "Build todos:"
		COMMAND echo "    cmake --build build -- todo"
		COMMAND echo "Build all:"
		COMMAND echo "    cmake --build build"
		COMMAND echo "Run sage:"
		COMMAND echo "    cmake --build build -- sage && ./build/bin/sage"
		COMMAND echo "Perf sage:"
		COMMAND echo "    cmake --build build -- perf"
		COMMAND echo "    ./build/perf/perf.sh"
		COMMAND echo "    firefox ./build/perf/perf.svg"
		VERBATIM
	)

