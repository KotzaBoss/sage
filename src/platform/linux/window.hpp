#pragma once

#include "src/window.hpp"
#include "src/input.hpp"

#include "repr.hpp"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

namespace sage::oslinux::inline window {

using Size = sage::Size<int>;

struct Window : sage::window::Base {

private:
	GLFWwindow* _glfw;

public:
	Window(sage::window::Properties&& props)
		: Base{std::move(props)}
	{
		const auto ok = glfwInit();
		SAGE_ASSERT(ok);

		_glfw = glfwCreateWindow(
			_properties.size.width,
			_properties.size.height,
			_properties.title.c_str(),
			nullptr,
			nullptr
			);
		SAGE_ASSERT(_glfw);
	}

public:
	auto setup() -> void {
		glfwMakeContextCurrent(_glfw);

		const auto version = gladLoadGL(glfwGetProcAddress);
		SAGE_ASSERT(version);

		SAGE_LOG_INFO("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

		// User Data
		glfwSetWindowUserPointer(_glfw, this);

		// Callbacks

		// No captures, must be convertible to functon
		glfwSetWindowCloseCallback(_glfw, [] (GLFWwindow* win) {
				user_pointer_to_this_ref(win)
					._pending_event.assign(Event::make_window_closed());
			});

		glfwSetWindowSizeCallback(_glfw, [] (GLFWwindow* win, int width, int height) {
				auto& _this = user_pointer_to_this_ref(win);

				_this._properties.size = Size::to<size_t>(Size{width, height});
				_this._pending_event.assign(Event::make_window_resized(_this._properties.size));
			});

		glfwSetMouseButtonCallback(_glfw, [] (GLFWwindow* win, int button, int action, [[maybe_unused]] int mods) {
				user_pointer_to_this_ref(win)
					._pending_event.assign(Event::make_mouse_button({
						.type = std::invoke([&] { switch (action) {
								case GLFW_PRESS:	return Event::Type::Mouse_Button_Pressed;
								case GLFW_RELEASE:	return Event::Type::Mouse_Button_Released;
								default:			return Event::Type::None;
						}}),
						.mouse_button = std::invoke([&] { switch (button) {
								case GLFW_MOUSE_BUTTON_LEFT:	return input::Mouse::Button::Left;
								case GLFW_MOUSE_BUTTON_RIGHT:	return input::Mouse::Button::Right;
								case GLFW_MOUSE_BUTTON_MIDDLE:	return input::Mouse::Button::Middle;
								default:						return input::Mouse::Button::None;
						}})
					})
				);
			});
	}

	auto update() -> void {
		glfwPollEvents();
		glfwSwapBuffers(_glfw);
	}

	auto teardown() -> void {
		glfwDestroyWindow(_glfw);
		glfwTerminate();
	}

	auto native_handle() const -> GLFWwindow* {
		return _glfw;
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
