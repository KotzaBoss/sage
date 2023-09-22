#pragma once

#include "std.hpp"

#include "src/input.hpp"
#include "src/event.hpp"
#include "src/math.hpp"

#include "GLFW/glfw3.h"

namespace sage::glfw {

// Make sure its constructed before any other glfw operations are performed.
struct Handle {
private:
	inline static auto flag = std::once_flag{};

public:
	Handle() {
		const auto ok = glfwInit();	// glfwInit/glfwTerminate just returns if already initialized
		SAGE_ASSERT(ok);

		std::call_once(flag, [] {
				glfwSetErrorCallback([] (int err, const char* msg) {
					SAGE_LOG_ERROR("{} ({:#}): {}", err, err, msg);
				});
			});
	}

	~Handle() {
		glfwTerminate();
	}
};

inline namespace input {

namespace action {

constexpr auto is_expected(const int a) -> bool {
	return a == GLFW_RELEASE
		or a == GLFW_PRESS
		or a == GLFW_REPEAT
		;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
// To get the biutiful "modern C" array intialization
// we have to bend the style and not use auto otherwise GCC-13
// gets confused.
constexpr Event::Type mouse_button_type_map[] = {
	[GLFW_RELEASE]	= Event::Type::Mouse_Button_Released,
	[GLFW_PRESS]	= Event::Type::Mouse_Button_Pressed,
	[GLFW_REPEAT]	= Event::Type::Mouse_Button_Repeated,
};

constexpr Event::Type key_type_map[] = {
	[GLFW_RELEASE]	= Event::Type::Key_Released,
	[GLFW_PRESS]	= Event::Type::Key_Pressed,
	[GLFW_REPEAT]	= Event::Type::Key_Repeated,
};
#pragma GCC diagnostic pop

}// action

namespace mouse {

constexpr auto is_expected(const int m) -> bool {
	return m == GLFW_MOUSE_BUTTON_LEFT
		or m == GLFW_MOUSE_BUTTON_RIGHT
		or m == GLFW_MOUSE_BUTTON_MIDDLE
		;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
constexpr sage::input::Mouse::Button button_map[] = {
	[GLFW_MOUSE_BUTTON_LEFT]	= sage::input::Mouse::Button::Left,
	[GLFW_MOUSE_BUTTON_RIGHT]	= sage::input::Mouse::Button::Right,
	[GLFW_MOUSE_BUTTON_MIDDLE]	= sage::input::Mouse::Button::Middle,
};
#pragma GCC diagnostic pop

}// mouse

namespace key {

constexpr auto is_expected(const int m) -> bool {
	return m == GLFW_KEY_Q
		or m == GLFW_KEY_E
		or m == GLFW_KEY_W
		or m == GLFW_KEY_A
		or m == GLFW_KEY_S
		or m == GLFW_KEY_D
		;
}

inline struct Map {
private:
	// Yes wastefull but makes the user code simple. Can optimize later,
	// maybe with flat_map?
	std::array<sage::input::Key, GLFW_KEY_LAST + 1> _map;

public:
	Map() {
		_map[GLFW_KEY_A]			= sage::input::Key::A;
		_map[GLFW_KEY_D]			= sage::input::Key::D;
		_map[GLFW_KEY_E]			= sage::input::Key::E;
		_map[GLFW_KEY_Q]			= sage::input::Key::Q;
		_map[GLFW_KEY_S]			= sage::input::Key::S;
		_map[GLFW_KEY_W]			= sage::input::Key::W;
		_map[GLFW_KEY_LEFT_CONTROL]	= sage::input::Key::Left_Ctrl;
	}

public:
	auto operator[] (const int k) const -> sage::input::Key {
		return _map[k];
	}

} map;

}// key

}// input

}// glfw

