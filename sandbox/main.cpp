#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "sage.hpp"

#include "platform/linux/window.hpp"
#include "layer_sandbox.hpp"

using namespace sage;

TEST_CASE ("App") {
	using App = sage::App<
			oslinux::Window,
			Layer_Sandbox
		>;

	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	auto camera_controller = camera::Controller{input};
	auto renderer = oslinux::Renderer{};
	auto layer_sandbox = Layer_Sandbox{renderer, camera_controller};
	auto app = App {
			std::move(win),
			std::move(layer_sandbox)
		};

	SAGE_LOG_INFO(app);

	app.start();

	static auto exit = false;
	signal(SIGINT, [] (int) { exit = true; });
	while (not exit)
		;

	app.stop();
}
