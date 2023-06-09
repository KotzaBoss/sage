#pragma once

#include "src/window.hpp"

#include "GLFW/glfw3.h"

namespace sage::oslinux::inline window {

struct Window : sage::Window {

private:
	GLFWwindow* glfw;

public:
	Window(Properties&& props);

public:
	friend auto operator<< (std::ostream& o, const oslinux::Window& w) -> std::ostream& {
		return o << static_cast<const sage::Window&>(w);
	}
};

}// sage::linux::window
