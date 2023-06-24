#pragma once

#include <thread>

#include "window.hpp"
#include "layer.hpp"
#include "formatter.hpp"

#include "repr.hpp"

namespace sage::inline app {

template<window::Concept Window>
struct App {

private:
	Window _window;
	Layers layers;
	std::jthread loop;

public:
	App(window::Properties&& properties, Layers&& ls)
		: _window{std::move(properties)}
		, layers{std::move(ls)}
	{}

	~App() {
		SAGE_ASSERT_MSG(not loop.joinable(), "Make sure you have both start and stop in place");
	}

public:
	auto start() -> void {
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
	auto stop() -> void {
		loop.request_stop();
		loop.join();
	}

	auto event_callback(const Event& e) -> void {
		SAGE_LOG_INFO("App.event_callback: {}", e);
		layers.event_callback(e);
	}

public:
	auto window() const -> const Window& { return _window; }

public:
	template<window::Concept _Window>
	friend
	REPR_DEF_FMT(App<_Window>)
};

}// sage

template <sage::window::Concept Window>
FMT_FORMATTER(sage::App<Window>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::App<Window>) {
		return fmt::format_to(ctx.out(), "App: window={};", obj.window());
	}
};

