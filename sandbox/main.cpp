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

	Camera_Layer(camera::Orthographic& c)
		: camera{c}
	{}

	auto setup() {}
	auto update() {}
	auto imgui_prepare() -> void {}
	auto teardown() {}
	auto event_callback(const Event& e) {
		switch (e.type) {
			case Event::Type::Key_Pressed:	[[fallthrough]];
			case Event::Type::Key_Repeated:	{
				SAGE_LOG_DEBUG("Camera_Layer.event_callback: {}", e);
				SAGE_LOG_DEBUG("Camera_Layer.event_callback: {}", camera);
				auto pos = camera.position();
				switch (std::get<input::Key>(e.payload)) {
					case input::Key::Up:	pos.y += 0.1f; break;
					case input::Key::Down:	pos.y -= 0.1f; break;
					case input::Key::Left:	pos.x -= 0.1f; break;
					case input::Key::Right:	pos.x += 0.1f; break;
					default:
						return;
				}
				camera.set_position(pos);
				return;
			}
			default:
				return;
		}
	}

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
	auto camera_layer = Camera_Layer{camera};
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
