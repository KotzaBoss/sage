#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "sage.hpp"

#include "platform/linux/window.hpp"
#include "layer_sandbox.hpp"
#include "layer_2d.hpp"

using namespace sage;

TEST_CASE ("App") {
	static auto stop_source = std::stop_source{};
	signal(SIGINT, [] (int) { stop_source.request_stop(); });

	//

	using App = sage::App<
			oslinux::Window,
			Layer_2D
		>;

	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	auto renderer = oslinux::Renderer_2D{};
	auto layer_2d = Layer_2D{input, renderer};
	auto app = App {
			std::move(win),
			std::move(layer_2d)
		};

	app.run(stop_source.get_token());
}
