section_start("test")

include(CTest)

file(GLOB TESTS *.test.cpp)
foreach (test ${TESTS})
	cmake_path(GET test FILENAME test)
	string(REPLACE ".cpp" "" bin ${test})

	add_executable(${bin} ${test})
	target_include_directories(${bin} PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
	target_link_libraries(${bin} PRIVATE log doctest repr)

	if (${bin} STREQUAL "event.test")
		target_link_libraries(${bin} PRIVATE event)
	elseif (${bin} STREQUAL "linux_window.test")
		target_link_libraries(${bin} PRIVATE linux_window event)
	elseif (${bin} STREQUAL "linux_input.test")
		target_link_libraries(${bin} PRIVATE linux_window event)
	elseif (${bin} STREQUAL "layer.test")
		target_link_libraries(${bin} PRIVATE layer_imgui event)
	elseif (${bin} STREQUAL "layer_imgui.test")
		target_link_libraries(${bin} PRIVATE layer_imgui linux_window event)
	endif()

	target_precompile_headers(${bin} REUSE_FROM std)

	add_test(${bin} ${bin})

	cmake_print_properties(TARGETS ${bin} PROPERTIES SOURCES LINK_LIBRARIES)
endforeach()

section_pass("Ok")
