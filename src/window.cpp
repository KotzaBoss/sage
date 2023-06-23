#include "window.hpp"

namespace sage::window {

Window::Window(Args&& args)
	: _properties{std::move(args.properties)}
	, _setup{std::move(args.setup)}
	, _update{std::move(args.update)}
	, _teardown{std::move(args.teardown)}
	, _event_callback{Event::uninitialized_callback}
{}


auto Window::setup(Event::Callback&& cb) -> void {
	_event_callback = std::move(cb);
	std::invoke(_setup);
}

auto Window::update() -> void {
	std::invoke(_update);
}

auto Window::teardown() -> void {
	std::invoke(_teardown);
	_event_callback = Event::uninitialized_callback;
}

auto Window::event_callback(const Event& e) -> void {
	std::invoke(_event_callback, e);
}

}// sage::window
