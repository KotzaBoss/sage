#include "app.hpp"

namespace sage {

App::App(Window& win)
	: window{win}
{}

auto App::run() -> void {
	window.setup();
	for (const auto _ : vw::iota(1, 10)) {
		window.update();
		std::this_thread::sleep_for(100ms);
	}
	window.teardown();
}

}// sage
