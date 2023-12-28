#pragma once

#include "src/window.hpp"

#include "src/repr.hpp"

#include "src/platform/linux/graphics.hpp"
#include "glfw.hpp"

namespace sage::oslinux::inline window {

using Size = math::Size<int>;

struct Window : sage::window::Base {

private:
	glfw::Handle handle;
	GLFWwindow* glfw = nullptr;
	OpenGL_Context context;

public:
	Window(sage::window::Properties&& props = {})
		: Base{std::move(props)}
		, glfw{glfwCreateWindow(
			props.size.width,
			props.size.height,
			props.title.data(),
			nullptr,
			nullptr
			)}
		, context{glfw}
	{
		SAGE_ASSERT(props.title.rfind('\0'), "OpenGL window title {:?} is not NULL terminated string", props.title);
		SAGE_ASSERT(glfw);

		glfwSwapInterval(1);	// vsync on

		// User Data
		// CAUTION: Should the object be moved the 'this' is no longer valid, therefore any move should reset the pointer.
		// There is no synchronisation so proceed with caution. You should move only before any App runs.
		glfwSetWindowUserPointer(glfw, this);

		// Callbacks

		// No captures, must be convertible to functon
		glfwSetWindowCloseCallback(glfw, [] (GLFWwindow* win) {
				user_pointer_to_this_ref(win)
					._pending_event.store(Event::make_window_closed());
			});

		glfwSetWindowSizeCallback(glfw, [] (GLFWwindow* win, int width, int height) {
				auto& _this = user_pointer_to_this_ref(win);

				atomic_delta_store(_this._properties, [=] (auto& p) {
						p.size = Size{width, height}.to<size_t>();
					});

				_this._pending_event.store(Event::make_window_resized(_this._properties.load().size));
			});

		glfwSetWindowIconifyCallback(glfw, [] (GLFWwindow* win, int iconified) {
				auto& _this = user_pointer_to_this_ref(win);

				atomic_delta_store(_this._properties, [=] (auto& p) {
						p.is_minimized = iconified;
					});

				if (iconified)
					_this._pending_event.store(Event::make_window_minimized());
				else
					_this._pending_event.store(Event::make_window_restored());
			});

		// TODO: Change switches to array lookups for clarity

		glfwSetMouseButtonCallback(glfw, [] (GLFWwindow* win, int button, int action, [[maybe_unused]] int mods) {
				SAGE_ASSERT(glfw::action::is_expected(action));
				SAGE_ASSERT(glfw::mouse::is_expected(button));

				user_pointer_to_this_ref(win)
					._pending_event.store(Event::make_mouse_button({
							.type = glfw::action::mouse_button_type_map[action],
							.mouse_button = glfw::mouse::button_map[button],
						})
					);
			});

		glfwSetScrollCallback(glfw, [] (GLFWwindow* win, double xoffset, double yoffset) {
				user_pointer_to_this_ref(win)
					._pending_event.store(Event::make_mouse_scroll({
								.offset = {
									.x = xoffset,
									.y = yoffset
								}
							})
						);
			});

		glfwSetKeyCallback(glfw, [] (GLFWwindow* win, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
				SAGE_ASSERT(glfw::action::is_expected(action));
				SAGE_ASSERT(glfw::key::is_expected(key));

				user_pointer_to_this_ref(win)
					._pending_event.store(Event::make_key({
									.type = glfw::action::key_type_map[action],
									.key = glfw::key::map[key],
								})
					);
			});
	}

	Window(Window&& other)
		: Base(std::move(other))
		, handle{std::move(other.handle)}
		, glfw{std::exchange(other.glfw, nullptr)}
		, context{std::move(other.context)}
	{
		SAGE_ASSERT(&other != this);
		glfwSetWindowUserPointer(glfw, this);
	}

	~Window() {
		if (glfw)
			glfwDestroyWindow(glfw);
	}

public:
	auto update() -> void {
		SAGE_ASSERT(glfw);

		glfwPollEvents();
		context.swap_buffers();
	}

	auto native_handle() const -> GLFWwindow* {
		return glfw;
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
