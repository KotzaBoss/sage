#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "sage.hpp"

#include "platform/linux/window.hpp"
#include "layer_sandbox.hpp"
#include "layer_2d.hpp"

using namespace sage;

TEST_CASE ("App") {
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

	SAGE_LOG_INFO(app);

	app.start();

	static auto exit = false;
	signal(SIGINT, [] (int) { exit = true; });
	while (not exit)
		;

	app.stop();
}
