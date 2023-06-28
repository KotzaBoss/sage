#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "platform/linux/window.hpp"
#include "platform/linux/input.hpp"

using namespace sage;

TEST_CASE ("Linux Window") {
	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	MESSAGE(win);
	win.setup();

	const auto end = std::chrono::steady_clock::now() + 2s;
	while (std::chrono::steady_clock::now() < end) {
		win.update();
		MESSAGE(
				"key: ", input.is_key_pressed(input::Key::Left_Ctrl), ", ",
				"mouse_button: ", input.is_mouse_button_pressed(input::Mouse::Button::Left), ", ",
				input.mouse_pos()
			);
	}
	win.teardown();
}
