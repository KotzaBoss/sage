#include "app.hpp"
#include "formatter.hpp"
#include "log.hpp"

namespace sage {

App::App(Window& win)
	: _window{win}
{}

App::~App() {
	SAGE_ASSERT(loop.joinable());
}

auto App::start() -> void {
	loop = std::jthread{[this] (const auto stoken) {
			_window.setup([this] (const Event& e) { event_callback(e); });
			while (not stoken.stop_requested()) {
				_window.update();
			}
			_window.teardown();
		}};
}

auto App::stop() -> void {
	loop.request_stop();
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
