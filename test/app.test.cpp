#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "sage.hpp"
#include "platform/linux/window.hpp"
#include "platform/linux/input.hpp"
#include "platform/linux/graphics.hpp"

#include "layer_imgui.hpp"
#include "placeholder_layers.hpp"

using namespace sage;

TEST_CASE ("App") {
	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	using App = sage::App<
			oslinux::Window,
			oslinux::Input,
			oslinux::Vertex_Array, oslinux::Vertex_Buffer, oslinux::Index_Buffer,
			oslinux::Renderer,
			oslinux::Shader,
			Other_Layer, Dump_Layer
		>;
	auto app = App{
			std::move(win),
			std::move(input),
			Dump_Layer{3},
			Other_Layer{1},
			Other_Layer{2},
			Dump_Layer{4}
		};
	MESSAGE(app);
	app.start();
	std::this_thread::sleep_for(3s);
	app.stop();
}
