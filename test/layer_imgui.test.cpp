#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "layer_imgui.hpp"
#include "platform/linux/window.hpp"

using namespace sage;

TEST_CASE ("ImGui Layer") {

	auto win = oslinux::Window(sage::window::Properties{});
	auto imgui = layer::ImGui(win.native_handle());

	MESSAGE(imgui);

	win.setup();
	imgui.setup();

	for (auto i = 0; i < 3; ++i) {
		imgui.update();
		win.update();
		std::this_thread::sleep_for(1s);
	}

	imgui.teardown();
	win.teardown();

}
