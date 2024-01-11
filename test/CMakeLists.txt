section_start("test")

include(CTest)

file(GLOB TESTS *.test.cpp)
foreach (test ${TESTS})
	cmake_path(GET test FILENAME test)		# some.test.cpp
	string(REPLACE ".cpp" "" bin ${test})	# some.test
	string(REPLACE ".test" "" name ${bin})	# some

	add_executable(${bin} ${test})
	target_compile_definitions(${bin} PRIVATE SAGE_$<UPPER_CASE:${name}>_TEST)
	target_include_directories(${bin} PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
	target_link_libraries(${bin} PRIVATE log doctest repr layer_imgui linux_window event)
	target_precompile_headers(${bin} REUSE_FROM std)

	add_test(${bin} ${bin})

	cmake_print_properties(TARGETS ${bin} PROPERTIES SOURCES LINK_LIBRARIES)
endforeach()

section_pass()
