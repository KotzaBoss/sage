#pragma once

#include "layer.hpp"


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "platform/linux/window.hpp"

namespace sage::layer {

struct ImGui {

private:
	float time = 0.0f;
	GLFWwindow* glfw;

public:
	ImGui(GLFWwindow* w)
		: glfw{w}
	{
		SAGE_ASSERT(glfw);
	}

	auto setup() -> void {
		::ImGui::CreateContext();
		::ImGui::StyleColorsDark();

		auto& io = ::ImGui::GetIO();
		io.BackendFlags |=
			ImGuiBackendFlags_HasMouseCursors
			| ImGuiBackendFlags_HasSetMousePos
			;

		ImGui_ImplGlfw_InitForOpenGL(glfw, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	auto update() -> void {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		::ImGui::NewFrame();

		auto& io = ::ImGui::GetIO();
		io.DisplaySize = ImVec2(1024, 768);

		// FIXME: Find a way to pass "sage context information" to the layer starting
		// here in the update function
		//

		auto now = glfwGetTime();
		io.DeltaTime = time > 0.0f ? (now - time) : (1.0f / 60.0f);
		time = now;

		static auto show = true;
		::ImGui::ShowDemoWindow(&show);

		::ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
	}

	auto teardown() -> void {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		::ImGui::DestroyContext();
	}

	auto event_callback(const Event& e) -> void {}

public:
	REPR_DECL(ImGui);
	friend FMT_FORMATTER(ImGui);
};

}// sage::layer

template <>
FMT_FORMATTER(sage::layer::ImGui) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::layer::ImGui) {
		return fmt::format_to(ctx.out(), "layer::ImGui: ");
	}
};
