file(GLOB src *.glsl)
add_custom_target(shaders SOURCES ${src})
add_custom_command(
		TARGET shaders
		COMMENT "Copying shader sources to ${CMAKE_CURRENT_BINARY_DIR}"
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src} ${CMAKE_CURRENT_BINARY_DIR}
	)
cmake_print_variables(src)
