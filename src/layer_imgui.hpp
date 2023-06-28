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

	auto event_callback(const Event& e) -> void {
		using namespace sage;

		static constexpr int sage_to_imgui_mouse_buttons[] = {
				[std::to_underlying(input::Mouse::Button::Left)] = ::ImGuiMouseButton_Left,
				[std::to_underlying(input::Mouse::Button::Right)] = ::ImGuiMouseButton_Right,
				[std::to_underlying(input::Mouse::Button::Middle)] = ::ImGuiMouseButton_Middle,
			};

		SAGE_LOG_INFO("layer::ImGui.event_callback: {}", e);

		switch (e.type) {

			case Event::Type::Mouse_Button_Pressed: {
				SAGE_ASSERT(std::holds_alternative<input::Mouse::Button>(e.payload));

				const auto button = std::get<input::Mouse::Button>(e.payload);
				const auto button_idx = std::to_underlying(button);
				SAGE_ASSERT_MSG(button_idx >= 0 and (size_t)button_idx < sizeof(sage_to_imgui_mouse_buttons),
						fmt::format("Index for mouse button {} is out of bounds", button)
					);

				::ImGui::GetIO()
					.AddMouseButtonEvent(sage_to_imgui_mouse_buttons[button_idx], true);

				return;
			}

			case Event::Type::Mouse_Button_Released: {
				SAGE_ASSERT(std::holds_alternative<input::Mouse::Button>(e.payload));

				const auto button = std::get<input::Mouse::Button>(e.payload);
				const auto button_idx = std::to_underlying(button);
				SAGE_ASSERT_MSG(button_idx >= 0 and (size_t)button_idx < sizeof(sage_to_imgui_mouse_buttons),
						fmt::format("Index for mouse button {} is out of bounds", button)
					);

				::ImGui::GetIO()
					.AddMouseButtonEvent(sage_to_imgui_mouse_buttons[button_idx], false);

				return;
			}

			case Event::Type::Window_Closed: {
				SAGE_LOG_INFO("layer::ImGui raising SIGINT");
				std::raise(SIGINT);
				return;
			}

			default:
				SAGE_LOG_WARN("Unexpected event type {}", e.type);
				SAGE_ASSERT(false);
		}
	}

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
