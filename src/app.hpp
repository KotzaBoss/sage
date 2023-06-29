#pragma once

#include "std.hpp"

#include "window.hpp"
#include "input.hpp"
#include "layer.hpp"
#include "layer_imgui.hpp"

#include "log.hpp"
#include "repr.hpp"

namespace sage::inline app {

template<window::Concept Window, input::Concept Input, sage::layer::Concept... Ls>
	requires (not same_as_any<layer::ImGui, Ls...>)	// The ImGui layer will always be provided by sage as the "overlay"
struct App {
	using Layers = sage::layer::Storage<layer::ImGui, Ls...>;

private:
	Window window;
	Input input;
	Layers layers;
	// We wont be moving the ImGui vector in the tuple so its ok to reference.
	// If something more complex is needed in the future, we should prefer an Iterator
	// which is re-assigned if we tweak the ImGui vector (dont forget vector iterators can get
	// invalidated).
	layer::ImGui& imgui;

	std::jthread loop;

public:
	App(Window&& w, Input&& i, same_as_any<Ls...>auto &&... ls)
		: window{std::move(w)}
		, input{std::move(i)}
		, layers{layer::ImGui{window}, std::move(ls)...}
		, imgui{layers.front()}
	{}

	~App() {
		SAGE_ASSERT_MSG(not loop.joinable(), "Make sure you have both start and stop in place");
	}

public:
	auto start() -> void {
		loop = std::jthread{[this] (const auto stoken) {

							// Setup
							// TODO: Move setup/teardown into constructors?
				window.setup();
				layers.setup();

							// Update
				while (not stoken.stop_requested()) {
					SAGE_LOG_TRACE(input.mouse_pos());

					if (const auto event = window.pending_event();
						event.has_value())
					{
						layers.event_callback(*event);
					}

					layers.update();

					imgui.new_frame([&] {
							layers.imgui_prepare();
						});

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

