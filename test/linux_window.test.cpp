#include "window.hpp"
using namespace sage;

auto main() -> int {
	auto& win = Window::make(Window::Properties{});
	std::cerr << win << '\n';
	win.setup([] (const Event& e) {
			std::cerr << e << '\n';
		});
	for (const auto _ : vw::iota(1, 7)) {
		win.update();
		std::this_thread::sleep_for(1s);
	}
	win.teardown();
}
