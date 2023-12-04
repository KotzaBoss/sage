#pragma once

#include "src/std.hpp"

#include "src/graphics.hpp"
#include "src/window.hpp"
#include "src/input.hpp"
#include "src/layer.hpp"
#include "src/layer_imgui.hpp"
#include "src/camera.hpp"
#include "src/perf.hpp"
#include "src/time.hpp"

#include "src/log.hpp"
#include "src/repr.hpp"

namespace sage::inline app {

template <
		window::Concept Window,
		input::Concept Input,
		graphics::renderer::Rendering R,
		layer::Spec... Ls
	>
	requires
			type::Not_In<layer::ImGui, typename Ls::Layer...>	// The ImGui layer will always be provided by sage as the "overlay"
		and type::All<typename Ls::Input...>
struct App {
	using Layers = sage::layer::Array<layer::ImGui::Spec<Input, R>, Ls...>;

private:
	Window window;

	Input input;

	Profiler profiler;

	R::Renderer renderer;

	Layers layers;

	layer::ImGui& imgui;

public:
	App(Window&& w, Input&& i)
		: window{std::move(w)}
		, input{std::move(i)}
		, profiler{{ .max_quads = R::Renderer::Batch::max_quads }}
		, renderer{profiler}
		, layers{layer::ImGui{&window}, typename Ls::Layer{}...}
		, imgui{layers.front()}
	{
		SAGE_LOG_DEBUG(*this);
	}

public:
	auto run(std::stop_token stoken) -> bool {
		for (auto tick = time::Tick{}; not stoken.stop_requested(); ) {
			PROFILER_TIME(profiler, "Frame");

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
				layers.update(delta, input);
				layers.render(renderer);
			}

			imgui.new_frame([this] {
					layers.imgui_prepare();
					profiler.imgui_prepare();
				});

			window.update();

			// TODO: Fixed rate updates how to? Hardcode to 144fps for now.
			// Look into: https://johnaustin.io/articles/2019/fix-your-unity-timestep
			const auto next_time_point = perf::target::time_point::_144(tick.current_time_point());
			std::this_thread::sleep_until(next_time_point);
		}

		return true;
	}

public:
	friend REPR_DEF_FMT(App<Window, Input, R, Ls...>)
	friend FMT_FORMATTER(App<Window, Input, R, Ls...>);
};

}// sage

template <typename... Ts>
FMT_FORMATTER(sage::App<Ts...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::App<Ts...>) {
		return fmt::format_to(ctx.out(), "App:\n\twindow={}\n\tlayers={}\n\t;", obj.window, obj.layers);
	}
};

