set(TESTS
	some.cpp
	)
foreach (test ${TESTS})
	string(REPLACE ".cpp" "" bin ${test})
	add_executable(${bin} ${test})
	
	target_include_directories(${bin}
		PRIVATE
			${PROJECT_SOURCE_DIR}/src
		)

	set_target_properties(${bin}
		PROPERTIES
			CXX_STANDARD		20
			CXX_STANDARD_REQUIRED	TRUE
			CXX_EXTENSIONS		TRUE
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
	
	#target_compile_definitions(${bin}
	#	PRIVATE
	#		GLEW_STATIC
	#	)
	
	target_link_libraries(${bin}
		PRIVATE
			app
	#		pthread
	#		glfw
	#		GL
	#		GLEW
		)
	
	add_test(${bin} ${bin})
endforeach()
