#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "platform/linux/window.hpp"

using namespace sage;

TEST_CASE ("Linux Window") {
	MESSAGE(window::Properties{});
	auto win = oslinux::Window(window::Properties{});
	MESSAGE(win);

	for (auto i = 0; i < 5; ++i) {
		win.update();
		std::this_thread::sleep_for(1s);
	}
}
