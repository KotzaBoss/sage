#pragma once

#include "src/window.hpp"
#include "repr.hpp"

#include "GLFW/glfw3.h"

namespace sage::oslinux::inline window {

using Size = sage::Size<int>;

struct Window final : sage::Window {

private:
	GLFWwindow* glfw;

public:
	Window(Properties&& props);

private:
	inline static auto user_pointer_to_this_ref(GLFWwindow* const win) -> Window& {
		return *static_cast<Window*>(glfwGetWindowUserPointer(win));
	}

public:
	REPR_DECL(Window);
};

}// sage::linux::window
