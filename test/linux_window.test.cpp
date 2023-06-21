#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "window.hpp"

using namespace sage;

TEST_CASE ("Linux Window") {
	auto& win = Window::make(Window::Properties{});
	MESSAGE(win);
	win.setup([] (const Event& e) {
			MESSAGE(e);
		});
	for (const auto _ : vw::iota(1, 7)) {
		win.update();
		std::this_thread::sleep_for(1s);
	}
	win.teardown();
}
