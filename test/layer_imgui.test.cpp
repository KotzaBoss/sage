#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test.hpp"

using namespace sage;

TEST_CASE ("ImGui Layer") {

	auto win = oslinux::Window(sage::window::Properties{});
	auto imgui = layer::ImGui(&win);

	MESSAGE(imgui);

	const auto start = std::chrono::steady_clock::now();
	for (auto tick = sage::Tick{}; tick.current_time_point() < start + 1s; ) {
		imgui.update(tick(), input::null);
		imgui.new_frame([&] {
				imgui.imgui_prepare();
			});
		win.update();
		std::this_thread::sleep_for(1s);
	}

}
