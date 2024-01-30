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
		graphics::renderer::Concept_2D Renderer,
		typename User_State,
		layer::Concept... Ls
	>
	requires
			type::Not_In<layer::ImGui<Input, Renderer, User_State>, Ls...>	// ImGui layer always provided as the "overlay"
		and std::constructible_from<User_State, ECS&>
struct App {
	using ImGui = layer::ImGui<Input, Renderer, User_State>;
	using Layers = sage::layer::Array<ImGui, Ls...>;
	using Camera_Controller = camera::Controller<Input>;

private:
	Window window;

	Input input;

	Profiler profiler;

	Renderer renderer;

	Camera_Controller camera_controller;

	Layers layers;

	ImGui& imgui;

	ECS ecs;

	User_State user_state;

public:
	App()
		: input{window.native_handle()}
		, profiler{{ .max_quads = Renderer::Batch::max_quads }}
		, renderer{profiler}
		, layers{ImGui{&window}, Ls{}...}
		, imgui{layers.front()}
		, ecs{1000ul}
		, user_state{ecs}
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
						PROFILER_TIME(profiler, "    Layers");

						layers.event_callback(*event, camera_controller, ecs, user_state);
					}

					{
						PROFILER_TIME(profiler, "    Renderer");

						renderer.event_callback(*event);
					}
				}
			}

			// As layers get more interesting this if may change but for now keep it
			// very strict: no window, no work
			if (not window.is_minimized()) {

				{
					PROFILER_TIME(profiler, "Update Layers");

					layers.update(delta, input, camera_controller, ecs, user_state);
				}

				{
					PROFILER_TIME(profiler, "Render Layers");

					renderer.scene(camera_controller.camera, [&] {
							layers.render(renderer, ecs, user_state);
						});
				}

				{
					PROFILER_TIME(profiler, "ImGui");

					imgui.new_frame([&] {
							layers.imgui_prepare(camera_controller, renderer.frame_buffer(), ecs, user_state);
						});
				}
			}

			window.update();

			const auto res = profiler.consume_results();
			SAGE_LOG_WARN(res);

			// TODO: Fixed rate updates how to? Hardcode to 144fps for now.
			// Look into: https://johnaustin.io/articles/2019/fix-your-unity-timestep
			const auto next_time_point = perf::target::fps144::next_time_point(tick.current_time_point());
			std::this_thread::sleep_until(next_time_point);
		}

		return true;
	}

public:
	friend REPR_DEF_FMT(App<Window, Input, Renderer, User_State, Ls...>)
	friend FMT_FORMATTER(App<Window, Input, Renderer, User_State, Ls...>);
};

}// sage

template <typename... Ts>
FMT_FORMATTER(sage::App<Ts...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::App<Ts...>) {
		return fmt::format_to(ctx.out(), "App:\n\twindow={}\n\tlayers={}\n\t;", obj.window, obj.layers);
	}
};

