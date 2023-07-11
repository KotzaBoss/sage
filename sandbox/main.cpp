#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "sage.hpp"
#include "platform/linux/window.hpp"
#include "platform/linux/input.hpp"
#include "platform/linux/graphics.hpp"

#include "camera.hpp"

#include "layer_imgui.hpp"

using namespace sage;

struct Camera_Layer {
	camera::Orthographic& camera;
	oslinux::Input& input;

	Camera_Layer(camera::Orthographic& c, oslinux::Input& i)
		: camera{c}
		, input{i}
	{}

	auto setup() {}
	auto update(const std::chrono::milliseconds dt) {
		constexpr auto movement = 5.f;
		const auto coeff = std::chrono::duration<float, std::chrono::seconds::period>{dt}.count();
		const auto pos_step = coeff * movement;

		auto pos = camera.position();
		if (input.is_key_pressed(input::Key::Up))			pos.y += pos_step;
		else if (input.is_key_pressed(input::Key::Down))	pos.y -= pos_step;
		else if (input.is_key_pressed(input::Key::Right))	pos.x += pos_step;

		if (input.is_key_pressed(input::Key::Left))	camera.set_rotation(camera.rotation() + coeff * 180.f);

		camera.set_position(pos);
	}
	auto imgui_prepare() -> void {}
	auto teardown() {}
	auto event_callback(const Event&) {}

	REPR_DECL(Camera_Layer);
};

template <>
FMT_FORMATTER(Camera_Layer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Camera_Layer) {
		return fmt::format_to(ctx.out(), "Camera_Layer: {}", obj.camera);
	}
};

REPR_DEF_FMT(Camera_Layer);

TEST_CASE ("App") {
	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	auto camera = camera::Orthographic{{ .left=-1.6f, .right=1.6f, .bottom=-0.9f, .top=0.9f }};
	auto camera_layer = Camera_Layer{camera, input};
	using App = sage::App<
			oslinux::Window,
			oslinux::Input,
			oslinux::Vertex_Array, oslinux::Vertex_Buffer, oslinux::Index_Buffer,
			oslinux::Renderer,
			oslinux::Shader,
			Camera_Layer
		>;
	auto app = App {
			std::move(win),
			std::move(input),
			camera,
			std::move(camera_layer)
		};
	SAGE_LOG_INFO(app);

	app.start();

	static auto exit = false;
	signal(SIGINT, [] (int) { exit = true; });
	while (not exit)
		;

	app.stop();
}
