add_library(linux_window INTERFACE window.hpp)
add_dependencies(linux_window glad)
target_link_libraries(linux_window
	INTERFACE
		glad
		glfw
		GL
		stb
	)
cmake_print_properties(TARGETS linux_window PROPERTIES SOURCES LINK_DIRECTORIES)

