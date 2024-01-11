section_start("test")

include(CTest)

file(GLOB tests RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test.*.cpp)
list(LENGTH tests len)
section_message("Number of tests ${len}")
if (SAGE_OPT_VERBOSE)
	cmake_print_variables(tests)
endif()

foreach (test ${tests})
	string(REPLACE ".cpp" "" bin ${test})	# test.some
	string(REPLACE "test." "" name ${bin})	# some

	add_executable(${bin} ${test})
	target_compile_definitions(${bin} PRIVATE SAGE_TEST_$<UPPER_CASE:${name}>)
	target_include_directories(${bin} PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
	target_link_libraries(${bin} PRIVATE log doctest repr layer_imgui linux_window event)
	target_precompile_headers(${bin} REUSE_FROM std)

	add_test(${bin} ${bin})
endforeach()

section_pass()
