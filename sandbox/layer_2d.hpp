#pragma once

#include "sage.hpp"

#include "platform/linux/input.hpp"
#include "platform/linux/graphics.hpp"

using namespace sage;

struct Layer_2D {
	using Camera_Controller = camera::Controller<oslinux::Input>;

public:
	Camera_Controller camera_controller;
	oslinux::Renderer_2D& renderer;
	oslinux::Texture2D texture;

public:
	Layer_2D(oslinux::Input& input, oslinux::Renderer_2D& r)
		: camera_controller{input}
		, renderer{r}
		, texture{"asset/texture/owl.png"}
	{}

public:
	auto update(const std::chrono::milliseconds delta) -> void {
		renderer.clear();

		renderer.scene(camera_controller.camera(), [&] {
				#pragma message "FIXME: Bit depth testing should allow all to be seen."
				renderer.draw({ -1.0f, 0.0f, 0.0f }, { 0.8f, 0.8f }, texture);
				renderer.draw({  0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, texture);
				renderer.draw({  0.5f, -0.5f, 0.0f }, { 0.5f, 0.75f }, glm::vec4{1.0, 1.0, 0.0, 1.0});
			});

		camera_controller.update(delta);
	}

	auto event_callback(const Event& e) -> void {
		camera_controller.event_callback(e);
	}

	auto imgui_prepare() -> void {
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
