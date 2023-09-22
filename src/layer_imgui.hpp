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
	oslinux::Window& window;

public:
	ImGui(oslinux::Window& w)
		: window{w}
	{
		::IMGUI_CHECKVERSION();		// 9000 IQ
		::ImGui::CreateContext();

		auto& io = ::ImGui::GetIO();
		io.ConfigFlags |=
			ImGuiConfigFlags_NavEnableKeyboard
			| ImGuiConfigFlags_DockingEnable
			| ImGuiConfigFlags_ViewportsEnable
			;

		::ImGui::StyleColorsDark();

		if (auto& style = ::ImGui::GetStyle();
			io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SAGE_ASSERT(window.native_handle() != nullptr);
		ImGui_ImplGlfw_InitForOpenGL(window.native_handle(), true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	auto update(const std::chrono::milliseconds) -> void {}

	auto teardown() -> void {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		::ImGui::DestroyContext();
	}

	auto event_callback(const Event& e) -> void {
		using namespace sage;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
		static constexpr int sage_to_imgui_mouse_buttons[] = {
				[std::to_underlying(input::Mouse::Button::Left)] = ::ImGuiMouseButton_Left,
				[std::to_underlying(input::Mouse::Button::Right)] = ::ImGuiMouseButton_Right,
				[std::to_underlying(input::Mouse::Button::Middle)] = ::ImGuiMouseButton_Middle,
			};
#pragma GCC diagnostic pop

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

			case Event::Type::Window_Resized:
				return;

			default:
				SAGE_LOG_DEBUG("layer::ImGui: Uninterested in event {}", e.type);
				return;
		}
	}

	// There can be different inputs as the engine proceeds, for example
	// a view so that it can be created by filtering etc.
	auto new_frame(const std::invocable auto& frame_work) -> void {
		// New Frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		::ImGui::NewFrame();

		// Other objects imgui work
		frame_work();

		// Render
		auto& io = ::ImGui::GetIO();
		const auto win_size = window.properties().size;
		io.DisplaySize = ImVec2(win_size.width, win_size.height);

		::ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			const auto context = glfwGetCurrentContext();
			::ImGui::UpdatePlatformWindows();
			::ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(context);
		}
	}

	auto imgui_prepare() {
		static auto show = true;
		::ImGui::ShowDemoWindow(&show);
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
		(void)obj;
		return fmt::format_to(ctx.out(), "layer::ImGui;");
	}
};
