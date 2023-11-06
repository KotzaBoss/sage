#pragma once

#include "std.hpp"

#include "graphics.hpp"
#include "window.hpp"
#include "input.hpp"
#include "layer.hpp"
#include "layer_imgui.hpp"
#include "camera.hpp"
#include "time.hpp"

#include "log.hpp"
#include "repr.hpp"

namespace sage::inline app {

template <
		window::Concept Window,
		graphics::renderer::Rendering R,
		layer::Spec... Ls
	>
	requires
		(not type::Any<layer::ImGui, typename Ls::Layer...>)	// The ImGui layer will always be provided by sage as the "overlay"
struct App {
	using Layers = sage::layer::Array<layer::ImGui::Spec<R>, Ls...>;

private:
	R::Renderer renderer;
	Window window;

	Layers layers;

	// We wont be moving the ImGui vector in the tuple so its ok to reference.
	// If something more complex is needed in the future, we should prefer an Iterator
	// which is re-assigned if we tweak the ImGui vector (dont forget vector iterators can get
	// invalidated).
	layer::ImGui& imgui;

public:
	App(Window&& w, type::Any<typename Ls::Layer...> auto&&... ls)
		: window{std::move(w)}
		, layers{layer::ImGui{&window}, std::move(ls)...}
		, imgui{layers.front()}
	{
		SAGE_LOG_DEBUG(*this);
	}

public:
	auto run(std::stop_token stoken) -> bool {
		for (auto tick = time::Tick{}; not stoken.stop_requested(); ) {
			const auto delta = tick();

			if (const auto event = window.consume_pending_event();
				event.has_value())
			{
				layers.event_callback(*event);
				renderer.event_callback(*event);
			}

			// As layers get more interesting this if may change but for now keep it
			// very strict: no window, no work
			if (not window.is_minimized()) {
				layers.update(delta);
				layers.render(renderer);
			}

			imgui.new_frame([this] {
					layers.imgui_prepare();
				});

			window.update();

			#pragma message "TODO: Fixed rate updates how to? Hardcode to 144fps for now."
			// Look into: https://johnaustin.io/articles/2019/fix-your-unity-timestep
			constexpr auto frame_duration = 1000ms / 144;
			std::this_thread::sleep_until(tick.current_time_point() + frame_duration);
		}

		return true;
	}

public:
	friend REPR_DEF_FMT(App<Window, R, Ls...>)
	friend FMT_FORMATTER(App<Window, R, Ls...>);
};

}// sage

template <typename... Ts>
FMT_FORMATTER(sage::App<Ts...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::App<Ts...>) {
		return fmt::format_to(ctx.out(), "App:\n\twindow={}\n\tlayers={}\n\t;", obj.window, obj.layers);
	}
};

