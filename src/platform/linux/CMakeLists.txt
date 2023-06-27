message(STATUS "linux_window")
message(STATUS "L glad")
message(STATUS "L glfw")
message(STATUS "L GL")
add_library(linux_window INTERFACE window.hpp)
add_dependencies(linux_window glad)
target_link_libraries(linux_window
	INTERFACE
		glad
		glfw
		GL
	)

