#pragma once

#include "std.hpp"

#include "src/input.hpp"

#include "GLFW/glfw3.h"

namespace sage::oslinux::inline input {

struct Input {
	using Key = sage::input::Key;
	using Mouse = sage::input::Mouse;

	static constexpr int key_map[] = {
			[std::to_underlying(Key::Left_Ctrl)] = GLFW_KEY_LEFT_CONTROL,
		};

	static constexpr int mouse_button_map[] = {
			[std::to_underlying(Mouse::Button::Left)] = GLFW_MOUSE_BUTTON_LEFT,
			[std::to_underlying(Mouse::Button::Right)] = GLFW_MOUSE_BUTTON_RIGHT,
			[std::to_underlying(Mouse::Button::Middle)] = GLFW_MOUSE_BUTTON_MIDDLE,
		};

private:
	GLFWwindow* glfw;

public:
	Input(GLFWwindow* w)
		: glfw{w}
	{}

public:
	auto is_key_pressed(const Key& k) -> bool {
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
