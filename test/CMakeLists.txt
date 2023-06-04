set(TESTS
	some.cpp
	event.cpp
	)
foreach (test ${TESTS})
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
	
	if (${bin} STREQUAL "some")
		target_link_libraries(${bin}
			PRIVATE
				app
				log
			)
	elseif (${bin} STREQUAL "event")
	endif()

	target_precompile_headers(${bin} REUSE_FROM app)
	
	add_test(${bin} ${bin})
endforeach()
