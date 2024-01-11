section_start("test")

include(CTest)

# Gave up using file(READ) for code, just fire up the shell
set(glob "*.hpp")
execute_process(
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		COMMAND grep -r --files-with-matches --include=${glob} "#ifdef SAGE_TEST_" src
		OUTPUT_VARIABLE testable_sources
	)

# Listify grep output
string(STRIP ${testable_sources} testable_sources)
string(REPLACE "\n" ";" testable_sources ${testable_sources})

list(LENGTH testable_sources len)
section_message("Number of tests ${len}")
cmake_print_variables(testable_sources)

foreach (src ${testable_sources})			# src/platform/linux/input.hpp
	string(REPLACE ".hpp" "" test ${src})	# src/platform/linux/input
	string(REPLACE "src/" "" test ${test})	# platform/linux/input
	string(REPLACE "/" "." test ${test})	# platform.linux.input

	set(bin "test.${test}")					# test.platform.linux.input

	set(test_src "${bin}.cpp")				# test.platform.linux.input.cpp

	string(REPLACE "." "_" ifdef ${test})	# platform_linux_input
	string(TOUPPER ${ifdef} ifdef)			# PLATFORM_LINUX_INPUT

	file(WRITE ${test_src}
			"#include \"test/doctest.hpp\"\n"
			"#include \"${src}\"\n"
		)

	add_executable(${bin} ${test_src})
	target_compile_definitions(${bin} PRIVATE SAGE_TEST_${ifdef})
	target_include_directories(${bin} PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
	target_link_libraries(${bin} PRIVATE log doctest repr layer_imgui linux_window event)
	target_precompile_headers(${bin} REUSE_FROM std)

	add_test(${bin} ${bin})

endforeach()

section_pass()
