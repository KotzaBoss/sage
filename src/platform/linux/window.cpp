#include "window.hpp"
#include "log.hpp"
#include "core.hpp"

namespace sage {

namespace window {

auto Window::make(Properties&& props) -> Window& {
	static auto win = oslinux::Window(std::move(props));
	return win;
}

}// window

namespace oslinux::window {

Window::Window(Properties&& props)
	: sage::Window{{
		.properties = std::move(props),
		.setup = [this] {
			const auto ok = glfwInit();
			SAGE_ASSERT(ok);

			glfw = glfwCreateWindow(
				properties.size.width,
				properties.size.height,
				properties.title.c_str(),
				nullptr,
				nullptr
				);
			glfwMakeContextCurrent(glfw);

			// User Data
			glfwSetWindowUserPointer(glfw, this);

			// Callbacks

			// No captures, must be convertible to functon
			glfwSetWindowCloseCallback(glfw, [] (GLFWwindow* win) {
				user_pointer_to_this_ref(win)
					.event_callback(Event::make_window_closed())
					;
				});

			glfwSetWindowSizeCallback(glfw, [] (GLFWwindow* win, int width, int height) {
				user_pointer_to_this_ref(win)
					.event_callback(Event::make_window_resized(Size{width, height}))
					;
				});
		},
		.update = [this] {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			glfwPollEvents();
			glfwSwapBuffers(glfw);
		},
		.teardown = [this] {
			glfwDestroyWindow(glfw);
		}
	}}
{}

}// oslinux::window

}// sage
