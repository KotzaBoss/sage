#pragma once

#include "src/sage.hpp"

#include "src/platform/linux/sage.hpp"

using namespace sage;

struct Game_State;

struct Editor {
	using Input = oslinux::Input;
	using Renderer = oslinux::Renderer_2D;
	using User_State = Game_State;	// TODO: How to "ignore" User_State

public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input, auto&) -> void {
	}

	auto render(oslinux::Renderer_2D& renderer, auto&) -> void {
	}

	auto event_callback(const Event& e, auto&) -> void {
	}

	auto imgui_prepare(camera::Controller<Input>& cam, Renderer::Frame_Buffer& frame_buffer, auto& state) -> void {
		::ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
		::ImGui::Begin("Rendering");

		const auto viewport_size = ::ImGui::GetContentRegionAvail();
		cam.resize({ viewport_size.x, viewport_size.y });
		frame_buffer.resize({viewport_size.x, viewport_size.y});
		::ImGui::Image(frame_buffer.color_attachment_id(), viewport_size, {0, 1}, {1, 0}); // {0, 1} {1, 0} to display it correctly and not inverted

		::ImGui::End();
		::ImGui::PopStyleVar();
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
