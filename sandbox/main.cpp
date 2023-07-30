#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "sage.hpp"
#include "platform/linux/window.hpp"
#include "platform/linux/input.hpp"
#include "platform/linux/graphics.hpp"

#include "camera.hpp"

#include "layer_imgui.hpp"

using namespace sage;

TEST_CASE ("App") {
	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	auto camera_controller = camera::Controller{1920.f / 1024.f, input};
	using App = sage::App<
			oslinux::Window,
			oslinux::Input,
			oslinux::Vertex_Array, oslinux::Vertex_Buffer, oslinux::Index_Buffer,
			oslinux::Renderer,
			oslinux::Shader
		>;
	auto app = App {
			std::move(win),
			std::move(input),
			camera_controller
		};
	SAGE_LOG_INFO(app);

	app.start();

	static auto exit = false;
	signal(SIGINT, [] (int) { exit = true; });
	while (not exit)
		;

	app.stop();
}
