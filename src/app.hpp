#pragma once

#include "std.hpp"

#include "window.hpp"
#include "input.hpp"
#include "layer.hpp"

#include "log.hpp"
#include "repr.hpp"

namespace sage::inline app {

template<window::Concept Window, input::Concept Input, sage::layer::Concept... Ls>
struct App {
	using Layers = sage::layer::Storage<Ls...>;

private:
	Window window;
	Input input;
	Layers layers;
	std::jthread loop;

public:
	App(Window&& w, Input&& i, Layers&& ls)
		: window{std::move(w)}
		, input{std::move(i)}
		, layers{std::move(ls)}
	{}

	~App() {
		SAGE_ASSERT_MSG(not loop.joinable(), "Make sure you have both start and stop in place");
	}

public:
	auto start() -> void {
		loop = std::jthread{[this] (const auto stoken) {

							// Setup
				window.setup();
				layers.setup();

							// Update
				while (not stoken.stop_requested()) {
					SAGE_LOG_INFO(input.mouse_pos());

					if (const auto event = window.pending_event();
						event.has_value())
					{
						SAGE_LOG_INFO("App.event_callback: {}", event);
						layers.event_callback(*event);
					}

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

public:
	friend REPR_DEF_FMT(App<Window, Input, Ls...>)
	friend FMT_FORMATTER(App<Window, Input, Ls...>);
};

}// sage

template <sage::window::Concept Window, sage::input::Concept Input, sage::layer::Concept... Ls>
FMT_FORMATTER(sage::App<Window, Input, Ls...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::App<Window, Input, Ls...>) {
		return fmt::format_to(ctx.out(), "App:\n\twindow={}\n\tlayers={}\n\t;", obj.window, obj.layers);
	}
};

