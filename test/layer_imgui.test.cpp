#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test/test.hpp"

using namespace sage;

TEST_CASE ("ImGui Layer") {

	auto win = oslinux::Window();
	auto imgui = layer::ImGui<input::Null, graphics::renderer::Null, layer::Null_User_State>(&win);

	const auto start = std::chrono::steady_clock::now();
	for (auto tick = sage::Tick{}; tick.current_time_point() < start + 1s; ) {
		imgui.update(tick(), input::null, layer::null_user_state);
		imgui.new_frame([&] {
				imgui.imgui_prepare(layer::null_user_state);
			});
		win.update();
		std::this_thread::sleep_for(1s);
	}

}
