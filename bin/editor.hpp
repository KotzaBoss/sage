#pragma once

#include "src/sage.hpp"

#include "src/platform/linux/sage.hpp"

using namespace sage;

struct Editor {
public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input, auto&) -> void {
	}

	auto render(oslinux::Renderer_2D& renderer, auto&) -> void {
	}

	auto event_callback(const Event& e, auto&) -> void {
	}

	template <typename App_State>
		requires (App_State as) {
			{ as.frame_buffer } -> std::same_as<const oslinux::Frame_Buffer&>;
		}
	auto imgui_prepare(App_State& state) -> void {
		ImGui::Begin("Editor");
		// {0, 1} {1, 0} to display it correctly and not inverted
		ImGui::Image(state.frame_buffer.color_attachment_id(), { 1280.f , 720.f}, {0, 1}, {1, 0});
		ImGui::End();
	}

	FMT_FORMATTER(Editor);
};

template <>
FMT_FORMATTER(Editor) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Editor) {
		(void)obj;
		return fmt::format_to(ctx.out(), "Editor;");
	}
};
