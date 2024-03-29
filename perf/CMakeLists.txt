section_start("perf")

include(FetchContent)

find_program(PERF perf)
if (NOT PERF)
	section_fail("${PERF}")
else()
	FetchContent_Declare(flamegraph
			GIT_REPOSITORY https://github.com/brendangregg/FlameGraph
			GIT_TAG origin/master
		)
	FetchContent_MakeAvailable(flamegraph)

	set(perf_script ${CMAKE_CURRENT_BINARY_DIR}/perf.sh)
	add_custom_target(perf
			DEPENDS sage
			BYPRODUCTS ${perf_script}
			COMMAND echo -n "" > ${perf_script}
			# cmake -E echo to evaluate the generator expression
			COMMAND cmake -E echo "${PERF} record --output=${CMAKE_CURRENT_BINARY_DIR}/perf.data -F 99 -a -g -- $<TARGET_FILE:sage>" >> ${perf_script}
			COMMAND echo "cd ${CMAKE_CURRENT_BINARY_DIR}" >> ${perf_script}
			COMMAND echo "${PERF} script | ${flamegraph_SOURCE_DIR}/stackcollapse-perf.pl >> out.perf-folded" >> ${perf_script}
			COMMAND echo "${flamegraph_SOURCE_DIR}/flamegraph.pl out.perf-folded > perf.svg" >> ${perf_script}
			COMMAND echo "cd - > /dev/null" >> ${perf_script}
			COMMAND echo "echo Run: firefox ${CMAKE_CURRENT_BINARY_DIR}/perf.svg" >> ${perf_script}
			COMMAND chmod +x ${perf_script}
			COMMENT "Preparing perf script: ${perf_script}"
			VERBATIM
		)

	docs(SET TARGET perf DOCS
			"cmake --build build -- perf && \\"
			"${perf_script} && \\"
			"firefox ${CMAKE_CURRENT_BINARY_DIR}/perf.svg"
		)

	section_pass(${perf_script} ${flamegraph_SOURCE_DIR})
endif()

