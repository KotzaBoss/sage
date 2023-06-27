#pragma once

#include "src/window.hpp"
#include "repr.hpp"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

namespace sage::oslinux::inline window {

using Size = sage::Size<int>;

struct Window : sage::window::Base {

private:
	GLFWwindow* glfw;

public:
	Window(sage::window::Properties&& props)
		: Base{std::move(props)}
	{}

public:
	auto setup() -> void {
		const auto ok = glfwInit();
		SAGE_ASSERT(ok);

		glfw = glfwCreateWindow(
			_properties.size.width,
			_properties.size.height,
			_properties.title.c_str(),
			nullptr,
			nullptr
			);
		glfwMakeContextCurrent(glfw);

		const auto version = gladLoadGL(glfwGetProcAddress);
		SAGE_ASSERT(version);

		SAGE_LOG_INFO("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

		// User Data
		glfwSetWindowUserPointer(glfw, this);

		// Callbacks

		// No captures, must be convertible to functon
		glfwSetWindowCloseCallback(glfw, [] (GLFWwindow* win) {
				user_pointer_to_this_ref(win)
					._pending_event.assign(Event::make_window_closed());
			});

		glfwSetWindowSizeCallback(glfw, [] (GLFWwindow* win, int width, int height) {
				auto& _this = user_pointer_to_this_ref(win);

				_this._properties.size = Size::to<size_t>(Size{width, height});
				_this._pending_event.assign(Event::make_window_resized(_this._properties.size));
			});
	}

	auto update() -> void {
		glClearColor(1, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		glfwSwapBuffers(glfw);
	}

	auto teardown() -> void {
		glfwDestroyWindow(glfw);
	}

private:
	static auto user_pointer_to_this_ref(GLFWwindow* const win) -> Window& {
		return *static_cast<Window*>(glfwGetWindowUserPointer(win));
	}

public:
	REPR_DECL(Window);
};

}// sage::linux::window

template <>
FMT_FORMATTER(sage::oslinux::Window) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::oslinux::Window) {
		return fmt::format_to(ctx.out(), "oslinux::Window: {}", obj.properties());
	}
};
