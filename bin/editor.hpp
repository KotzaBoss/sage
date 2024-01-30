#pragma once

#include "src/sage.hpp"

#include "src/platform/linux/sage.hpp"

using namespace sage;

struct Game_State;

struct Editor {
	using Input = oslinux::Input;
	using Renderer = oslinux::Renderer_2D;
	using User_State = Game_State;	// TODO: How to "ignore" User_State

private:
	struct {
		bool is_focused = false;
		bool is_hovered = false;
	} viewport;

public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input, camera::Controller<Input>& cam, ECS&, User_State&) -> void {
		if (viewport.is_focused)
			cam.update(delta, input);
	}

	auto render(oslinux::Renderer_2D& renderer, ECS&, auto&) -> void {
	}

	auto event_callback(const Event& e, camera::Controller<Input>& cam, ECS&, auto&) -> void {
		if (viewport.is_focused and viewport.is_hovered)
			cam.event_callback(e);
	}

	auto imgui_prepare(camera::Controller<Input>& cam, Renderer::Frame_Buffer& frame_buffer, ECS& ecs, auto& state) -> void {
		::ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
		::ImGui::Begin("Rendering");

		viewport = {
			.is_focused = ImGui::IsWindowFocused(),
			.is_hovered = ImGui::IsWindowHovered(),
		};

		const auto viewport_size = ::ImGui::GetContentRegionAvail();
		cam.resize(viewport_size);
		frame_buffer.resize(viewport_size);
		::ImGui::Image(frame_buffer.color_attachment_id(), viewport_size, {0, 1}, {1, 0}); // {0, 1} {1, 0} to display it correctly and not inverted

		rg::for_each(
				ecs.view<component::Camera>()
					| vw::filter([] (auto&& entt_cam) {
							return std::get<1>(entt_cam).has_value();
						}),
				[&] (auto&& entt_comps) {
					auto& cam = std::get<1>(entt_comps);
					SAGE_ASSERT(cam.has_value());

					if (not cam->has_fixed_aspect_ratio)
						cam->camera.set_viewport_size(viewport_size);
				}
			);


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
