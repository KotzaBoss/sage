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
			log
	#		pthread
	#		glfw
	#		GL
	#		GLEW
		)
	
	add_test(${bin} ${bin})
endforeach()
