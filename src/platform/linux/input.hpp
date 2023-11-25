#pragma once

#include "src/std.hpp"

#include "src/input.hpp"

#include "GLFW/glfw3.h"

namespace sage::oslinux::inline input {

struct Input {
	using Key = sage::input::Key;
	using Mouse = sage::input::Mouse;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	static constexpr int key_map[] = {
			[std::to_underlying(Key::Left_Ctrl)] = GLFW_KEY_LEFT_CONTROL,
			[std::to_underlying(Key::Space)] = GLFW_KEY_SPACE,
			[std::to_underlying(Key::Q)]	= GLFW_KEY_Q,
			[std::to_underlying(Key::E)]	= GLFW_KEY_E,
			[std::to_underlying(Key::W)]	= GLFW_KEY_W,
			[std::to_underlying(Key::A)]	= GLFW_KEY_A,
			[std::to_underlying(Key::S)]	= GLFW_KEY_S,
			[std::to_underlying(Key::D)]	= GLFW_KEY_D,
		};

	static auto is_in_key_map(const Key k) -> bool {
		const auto x = std::to_underlying(k);
		return 0 <= x and static_cast<size_t>(x) < sizeof(key_map);
	}


	static constexpr int mouse_button_map[] = {
			[std::to_underlying(Mouse::Button::Left)] = GLFW_MOUSE_BUTTON_LEFT,
			[std::to_underlying(Mouse::Button::Right)] = GLFW_MOUSE_BUTTON_RIGHT,
			[std::to_underlying(Mouse::Button::Middle)] = GLFW_MOUSE_BUTTON_MIDDLE,
		};
#pragma GCC diagnostic pop

private:
	GLFWwindow* glfw;

public:
	Input(GLFWwindow* w)
		: glfw{w}
	{}

public:
	auto is_key_pressed(const Key& k) -> bool {
		SAGE_ASSERT(is_in_key_map(k));
		const auto state = glfwGetKey(glfw, key_map[std::to_underlying(k)]);
		// TODO: Check error
		return state == GLFW_PRESS or state == GLFW_REPEAT;
	}

	auto is_mouse_button_pressed(const Mouse::Button& b) -> bool {
		const auto state = glfwGetMouseButton(glfw, mouse_button_map[std::to_underlying(b)]);
		// TODO: Check error
		return state == GLFW_PRESS;
	}

	auto mouse_pos() -> Mouse::Pos {
		double x, y;
		glfwGetCursorPos(glfw, &x, &y);
		return { .x = x, .y = y };
	}
};

}// sage::oslinux::input
