#pragma once

#include "src/window.hpp"

#include "repr.hpp"

#include "graphics.hpp"
#include "glfw.hpp"

namespace sage::oslinux::inline window {

using Size = math::Size<int>;

struct Window : sage::window::Base {

private:
	GLFWwindow* glfw;
	OpenGL_Context context;

public:
	Window(sage::window::Properties&& props)
		: Base{std::move(props)}
		, context{&glfw}
	{
		const auto ok = glfwInit();
		SAGE_ASSERT(ok);

		glfwSetErrorCallback([] (int err, const char* msg) {
				SAGE_LOG_ERROR("{} ({:#}): {}", err, err, msg);
			});

		const auto properties = _properties.load();
		SAGE_ASSERT_MSG(properties.title.rfind('\0'), "OpenGL window title {:?} is not NULL terminated string", properties.title);
		glfw = glfwCreateWindow(
			properties.size.width,
			properties.size.height,
			properties.title.data(),
			nullptr,
			nullptr
			);
		SAGE_ASSERT(glfw);
	}

public:
	auto setup() -> void {
		context.setup();

		glfwSwapInterval(1);	// vsync on

		// User Data
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

		#pragma message "TODO: Change switches to array lookups for clarity"

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

	auto update() -> void {
		glfwPollEvents();
		context.swap_buffers();
	}

	auto teardown() -> void {
		glfwDestroyWindow(glfw);
		glfwTerminate();
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
