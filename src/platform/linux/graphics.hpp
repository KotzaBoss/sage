#pragma once

#include "src/graphics.hpp"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

namespace sage::oslinux::inline graphics {

struct OpenGL_Context {
private:
	GLFWwindow** glfw;

public:
	OpenGL_Context(GLFWwindow** w)
		: glfw{w}
	{
		SAGE_ASSERT(glfw);
	}

public:
	auto setup() -> void {
		glfwMakeContextCurrent(*glfw);

		const auto version = gladLoadGL(glfwGetProcAddress);
		SAGE_ASSERT(version);

		SAGE_LOG_INFO("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
	}

	auto swap_buffers() -> void {
		glfwSwapBuffers(*glfw);
	}
};

}//sage::oslinux::graphics
