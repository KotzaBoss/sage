section_start("texture")

file(GLOB assets *)
list(FILTER assets EXCLUDE REGEX "CMakeLists.txt")
add_custom_target(textures SOURCES ${assets})
add_custom_command(
		TARGET textures
		COMMENT "Copying textures to ${CMAKE_CURRENT_BINARY_DIR}"
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${assets} ${CMAKE_CURRENT_BINARY_DIR}
	)

cmake_print_variables(assets)

section_pass()
