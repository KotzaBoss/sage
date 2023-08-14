#pragma once

#include "sage.hpp"

struct Layer_2D {
	using Camera_Controller = camera::Controller<oslinux::Input>;

public:
	Camera_Controller camera_controller;

	oslinux::Renderer_2D& renderer;

	glm::vec4 square_color = {0, 0, 0, 1};

public:
	Layer_2D(oslinux::Input& input, oslinux::Renderer_2D& r)
		: camera_controller{input}
		, renderer{r}
	{}

public:
	auto setup() -> void {
		renderer.setup();
	}

	auto update(const std::chrono::milliseconds delta) -> void {
		renderer.clear();

		renderer.scene(camera_controller.camera(), [&] {
				if (square_color.r < 1)
					square_color.r += 0.001f;

				renderer.draw({/* TODO */}, {/* TODO */}, square_color);
			});

		camera_controller.update(delta);
	}

	auto event_callback(const Event& e) -> void {
		camera_controller.event_callback(e);
	}

	auto teardown() -> void {
		renderer.teardown();
	}

	auto imgui_prepare() -> void {
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square color", glm::value_ptr(square_color));
		ImGui::End();
	}

};

template <>
FMT_FORMATTER(Layer_2D) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Layer_2D) {
		(void)obj;
		return fmt::format_to(ctx.out(), "Layer_2D;");
	}
};
