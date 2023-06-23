#include "std.hpp"

#include "app.hpp"
#include "formatter.hpp"
#include "log.hpp"

namespace sage {

App::App(Window& win, Layers&& ls)
	: _window{win}
	, layers{std::move(ls)}
{}

App::~App() {
	SAGE_ASSERT_MSG(not loop.joinable(), "Make sure you have both start and stop in place");
}

auto App::start() -> void {
	loop = std::jthread{[this] (const auto stoken) {

						// Setup
			_window.setup([this] (const Event& e) { event_callback(e); });
			layers.setup();

						// Update
			while (not stoken.stop_requested()) {
				layers.update();
				_window.update();
			}

						// Teardown
			layers.teardown();
			_window.teardown();
		}};
}

auto App::stop() -> void {
	loop.request_stop();
	loop.join();
}

auto App::event_callback(const Event& e) -> void {
	SAGE_LOG_INFO("Window.event_callback( {} )", e);
	layers.event_callback(e);
}

}// sage
