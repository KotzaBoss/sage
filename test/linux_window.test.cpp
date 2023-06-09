#include "window.hpp"

using namespace sage;

auto main() -> int {
	auto& win = Window::make(Window::Properties{});
	std::cerr << win << '\n';
	win.setup();
	for (const auto _ : vw::iota(1, 10)) {
		win.update();
		std::this_thread::sleep_for(100ms);
	}
	win.teardown();
}
