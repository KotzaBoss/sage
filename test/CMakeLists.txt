file(GLOB TESTS
	*.test.cpp
	)
foreach (test ${TESTS})
	cmake_path(GET test FILENAME test)
	string(REPLACE ".cpp" "" bin ${test})
	add_executable(${bin} ${test})
	
	target_include_directories(${bin}
		PRIVATE
			${PROJECT_SOURCE_DIR}/src
		)
	
	target_compile_options(${bin}
		PRIVATE
			-Wall
			-Wextra
			-Wpedantic
			-fsanitize=undefined,address
		)

	target_link_options(${bin}
		PRIVATE
			-fsanitize=undefined,address
		)

	target_link_libraries(${bin}
		PRIVATE
			log
			doctest
			repr
		)

	if (${bin} STREQUAL "app.test")
		target_link_libraries(${bin}
			PRIVATE
				glfw
				GL
				event
				layer
			)
	elseif (${bin} STREQUAL "event.test")
		target_link_libraries(${bin}
			PRIVATE
				event
			)
	elseif (${bin} STREQUAL "linux_window.test")
		target_link_libraries(${bin}
			PRIVATE
				glfw
				GL
				event
			)
	elseif (${bin} STREQUAL "layer.test")
		target_link_libraries(${bin}
			PRIVATE
				layer
			)
	endif()

	target_precompile_headers(${bin} REUSE_FROM std)
	
	add_test(${bin} ${bin})
endforeach()
