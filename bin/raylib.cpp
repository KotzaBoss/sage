#include "src/std.hpp"

inline namespace rl {
#include "raylib.h"
}

#include "imgui.h"
namespace imgui = ::ImGui;

#include "rlImGui.h"


namespace sage {
struct Window {
	Window() {
		rl::InitWindow(1024, 768, "Some window");
	}

	~Window() {
		rl::CloseWindow();
	}

	auto should_close() const -> bool {
		return rl::WindowShouldClose();
	}
};

struct ImGui {
	ImGui() {
		rlImGuiSetup(true);
		auto& io = imgui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	~ImGui() {
		rlImGuiShutdown();
	}

	template <std::invocable Fn>
	auto work(Fn&& fn) const -> void {
		rlImGuiBegin();

		// Always at the window origin
		imgui::SetNextWindowPos(ImVec2(0, 0));

		// Always at the window size
		imgui::SetNextWindowSize(ImVec2(float(rl::GetScreenWidth()), float(rl::GetScreenHeight())));

		// We just want to use this window as a host for the menubar and docking
		// so turn off everything that would make it act like a window
		const auto windowFlags =
				ImGuiWindowFlags_NoBringToFrontOnFocus
			|	ImGuiWindowFlags_NoNavFocus
			|	ImGuiWindowFlags_NoDocking
			|	ImGuiWindowFlags_NoTitleBar
			|	ImGuiWindowFlags_NoResize
			|	ImGuiWindowFlags_NoMove
			|	ImGuiWindowFlags_NoCollapse
			|	ImGuiWindowFlags_MenuBar
				// We want our game content to show through this window, so turn off the background.
			|	ImGuiWindowFlags_NoBackground
			;

		// We don't want any padding for windows docked to this window frame
		imgui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (imgui::Begin("Main", NULL, windowFlags)) {
			imgui::PopStyleVar();

			// Create a docking space inside our inner window that lets prevents anything from docking in the central node (so we can see our game content)
			imgui::DockSpace(::ImGui::GetID("Dockspace"), ImVec2(0.0f, 0.0f),  ImGuiDockNodeFlags_PassthruCentralNode);

			std::invoke(std::forward<Fn>(fn));

			imgui::End();
		}

		rlImGuiEnd();
	}
};

struct SAGE {
	Window window;
	ImGui imgui;

	auto run() {

		while (not window.should_close()) {
			rl::BeginDrawing();

			rl::ClearBackground(BLACK);
			rl::DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

			imgui.work([] {
				imgui::ShowDemoWindow(NULL);
			});

			rl::EndDrawing();
		}

	}
};

}// sage

using namespace sage;

auto main() -> int {
	auto sage = SAGE{};
	sage.run();
}
