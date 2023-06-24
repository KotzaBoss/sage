#pragma once

#include "std.hpp"

#include "window.hpp"
#include "layer.hpp"

#include "log.hpp"
#include "repr.hpp"

namespace sage::inline app {

template<window::Concept Window>
struct App {

private:
	Window window;
	Layers layers;
	std::jthread loop;

public:
	App(window::Properties&& properties, Layers&& ls)
		: window{std::move(properties)}
		, layers{std::move(ls)}
	{}

	~App() {
		SAGE_ASSERT_MSG(not loop.joinable(), "Make sure you have both start and stop in place");
	}

public:
	auto start() -> void {
		loop = std::jthread{[this] (const auto stoken) {

							// Setup
				window.setup([this] (const Event& e) { event_callback(e); });
				layers.setup();

							// Update
				while (not stoken.stop_requested()) {
					layers.update();
					window.update();
				}

							// Teardown
				layers.teardown();
				window.teardown();
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
	friend REPR_DEF_FMT(App<Window>)
	friend FMT_FORMATTER(App<Window>);
};

}// sage

template <sage::window::Concept Window>
FMT_FORMATTER(sage::App<Window>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::App<Window>) {
		return fmt::format_to(ctx.out(), "App:\n\twindow={}\n\tlayers={}\n\t;", obj.window, "LAYERS PLACEHOLDER");
	}
};

