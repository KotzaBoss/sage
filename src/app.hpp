#pragma once

#include "src/std.hpp"

#include "src/graphics.hpp"
#include "src/particle.hpp"
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
		typename User_State,
		layer::Spec... Ls
	>
	requires
			type::Not_In<layer::ImGui, typename Ls::Layer...>	// The ImGui layer will always be provided by sage as the "overlay"
struct App {
	using Layers = sage::layer::Array<layer::ImGui::Spec<Input, R, User_State>, Ls...>;
	using Camera_Controller = camera::Controller<Input>;

private:
	Window window;

	Input input;

	Profiler profiler;

	R::Renderer renderer;

	Camera_Controller camera_controller;

	User_State user_state;

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
			const auto delta = tick();

			{
				PROFILER_TIME(profiler, "Event Callbacks");

				if (const auto event = window.consume_pending_event();
					event.has_value())
				{
					{
						PROFILER_TIME(profiler, "	Layers");

						layers.event_callback(*event, user_state);
					}

					{
						PROFILER_TIME(profiler, "	Camera Controller");

						camera_controller.event_callback(*event);
					}


					{
						PROFILER_TIME(profiler, "	Renderer");

						renderer.event_callback(*event);
					}
				}
			}

			// As layers get more interesting this if may change but for now keep it
			// very strict: no window, no work
			if (not window.is_minimized()) {

				{
					PROFILER_TIME(profiler, "Update Layers");

					layers.update(delta, input, user_state);
				}

				{
					PROFILER_TIME(profiler, "Update Camera Controller");

					camera_controller.update(delta, input);
				}

				{
					PROFILER_TIME(profiler, "Render Layers");

					renderer.clear();
					renderer.scene(camera_controller.camera(), [&] {
							layers.render(renderer, user_state);
						});
				}
			}

			{
				PROFILER_TIME(profiler, "ImGui");

				imgui.new_frame([this] {
						layers.imgui_prepare();
					});
			}

			window.update();

			const auto res = profiler.consume_results();
			SAGE_LOG_WARN(res);

			// TODO: Fixed rate updates how to? Hardcode to 144fps for now.
			// Look into: https://johnaustin.io/articles/2019/fix-your-unity-timestep
			const auto next_time_point = perf::target::time_point::_144(tick.current_time_point());
			std::this_thread::sleep_until(next_time_point);
		}

		return true;
	}

public:
	friend REPR_DEF_FMT(App<Window, Input, R, User_State, Ls...>)
	friend FMT_FORMATTER(App<Window, Input, R, User_State, Ls...>);
};

}// sage

template <typename... Ts>
FMT_FORMATTER(sage::App<Ts...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::App<Ts...>) {
		return fmt::format_to(ctx.out(), "App:\n\twindow={}\n\tlayers={}\n\t;", obj.window, obj.layers);
	}
};

