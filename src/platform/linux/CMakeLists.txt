set(SRC
	#window.cpp
	)
foreach (src ${SRC})
	string(REPLACE ".cpp" "" lib linux_${src})

	message(STATUS "${lib}")
	add_library(${lib} SHARED ${src})

	message(STATUS "L window")
	message(STATUS "L glfw")
	message(STATUS "L GL")
	target_link_libraries(${lib}
		PUBLIC
			window
		PRIVATE
			glfw
			GL
	)
	target_precompile_headers(${lib} REUSE_FROM std)
endforeach()

