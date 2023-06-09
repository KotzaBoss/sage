set(SRC
	window.cpp
	)
foreach (src ${SRC})
	string(REPLACE ".cpp" "" lib linux_${src})
	add_library(${lib} SHARED ${src})
	target_link_libraries(${lib}
		PRIVATE
			log
			window
			glfw
			GL
	)
	target_precompile_headers(${lib} REUSE_FROM std)
endforeach()

