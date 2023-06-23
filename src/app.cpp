#include "app.hpp"
#include "formatter.hpp"

namespace sage {

App::App(Window& win)
	: window{win}
{}

auto App::run() -> void {
	window.setup([this] (const Event& e) { event_callback(e); });
	for (const auto _ : vw::iota(1, 10)) {
		window.update();
		std::this_thread::sleep_for(100ms);
	}
	window.teardown();
}

auto App::event_callback(const Event& e) -> void {
	switch (e.type) {
		case Event::Type::Window_Closed:	[[fallthrough]];
		case Event::Type::Window_Resized:
			SAGE_LOG_INFO("Window.event_callback( {} )", e);
			return;

		default:
			SAGE_ASSERT_MSG(false, "Event not expected");
			SAGE_LOG_WARN("Unexpected event: {}", e);
			return;
	}
}

}// sage
