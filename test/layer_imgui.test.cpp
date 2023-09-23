#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "layer_imgui.hpp"
#include "time.hpp"
#include "platform/linux/window.hpp"

using namespace sage;

TEST_CASE ("ImGui Layer") {

	auto win = oslinux::Window(sage::window::Properties{});
	auto imgui = layer::ImGui(&win);

	MESSAGE(imgui);

	const auto start = std::chrono::steady_clock::now();
	for (auto tick = sage::Tick{}; tick.current_time_point() < start + 1s; ) {
		imgui.update(tick());
		imgui.new_frame([&] {
				imgui.imgui_prepare();
			});
		win.update();
		std::this_thread::sleep_for(1s);
	}

}
