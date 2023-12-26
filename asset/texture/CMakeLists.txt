section_start("texture")

# FIXME: Copying all the assets with each build is a bit much but i wont bother with cmake
#        until its clearly taking time.
add_custom_target(textures
		ALL
		COMMENT "Copying textures to ${CMAKE_CURRENT_BINARY_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	)

cmake_print_variables(assets)

section_pass()
