#pragma once

#include "std.hpp"

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
		input::Concept Input,
		typename Vertex_Array, typename Vertex_Buffer, typename Index_Buffer,
		typename Renderer,
		graphics::shader::Concept Shader,
		sage::layer::Concept... Ls
	>
	requires
		graphics::array::vertex::Concept<Vertex_Array, Vertex_Buffer, Index_Buffer>
		and graphics::renderer::Concept<Renderer, Vertex_Array, Vertex_Buffer, Index_Buffer, Shader>
		and (not same_as_any<layer::ImGui, Ls...>)	// The ImGui layer will always be provided by sage as the "overlay"
struct App {
	using Layers = sage::layer::Storage<layer::ImGui, Ls...>;
	using Clock = std::chrono::steady_clock;

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

	Vertex_Array vertex_array, square_vertex_array;
	Renderer renderer;

	Shader shader, square_shader;

	camera::Orthographic& camera;

public:
	App(Window&& w, Input&& i, camera::Orthographic& c, same_as_any<Ls...>auto &&... ls)
		: window{std::move(w)}
		, input{std::move(i)}
		, layers{layer::ImGui{window}, std::move(ls)...}
		, imgui{layers.front()}
		, camera{c}
	{}

	~App() {
		SAGE_ASSERT_MSG(not loop.joinable(), "Make sure you have both start and stop in place");
	}

public:
	auto start() -> void {
		loop = std::jthread{[this] (const auto stoken) {
				setup();

				for (auto tick = time::Tick{}; not stoken.stop_requested(); ) {
					const auto delta = tick();

					if (const auto event = window.pending_event();
						event.has_value())
					{
						layers.event_callback(*event);
					}

					renderer.clear();

					renderer.scene(camera, [&] {
							renderer.submit(shader, vertex_array);
							const auto scale = glm::scale(glm::mat4{1}, glm::vec3{0.1f});
							for (auto y = 0; y < 20; ++y) {
								for (auto x = 0; x < 20; ++x) {
									const auto transform = glm::translate(glm::mat4{1}, glm::vec3{x * 0.11f, y * 0.11f, 0}) * scale;
									renderer.submit(square_shader, square_vertex_array, transform);
								}
							}
						});

					layers.update(delta);

					imgui.new_frame([&] {
							layers.imgui_prepare();
						});

					window.update();

					// TODO: Fixed rate updates how to? Hardcode to 144fps for now.
					// Look into: https://johnaustin.io/articles/2019/fix-your-unity-timestep
					constexpr auto frame_duration = 1000ms / 144;
					std::this_thread::sleep_until(tick.current_time_point() + frame_duration);
				}

				teardown();

			}};
	}
	auto stop() -> void {
		loop.request_stop();
		loop.join();
	}

private:
	auto setup() -> void {
					// TODO: Move setup/teardown into constructors?
		window.setup();
		layers.setup();

		auto vertex_buffer = Vertex_Buffer{};
		vertex_buffer.setup(
				{
					-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.f,
					 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.f,
					 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.f,
				},
				graphics::buffer::Layout{
					graphics::buffer::Element{{
							.name = "a_Position",
							.type = graphics::shader::data::Type::Float3
						}},
					graphics::buffer::Element{{
							.name = "a_Color",
							.type = graphics::shader::data::Type::Float4
					}},
				}
			);

		auto index_buffer = Index_Buffer{};
		index_buffer.setup({0u, 1u, 2u});

		vertex_array.setup(std::move(vertex_buffer), std::move(index_buffer));

		shader.setup(
			R"(
				#version 330 core

				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Color;

				uniform mat4 u_ViewProjection;
				uniform mat4 u_Transform;

				out vec3 v_Position;
				out vec4 v_Color;

				void main() {
					v_Position = a_Position;
					v_Color = a_Color;
					gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
				}
			)",
			R"(
				#version 330 core

				layout(location = 0) out vec4 color;

				in vec3 v_Position;
				in vec4 v_Color;

				void main() {
					color = vec4(v_Position, 1.0);
					color = v_Color;
				}
			)"
		);

		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////

		auto square_vertex_buffer = Vertex_Buffer{};
		square_vertex_buffer.setup(
				{
					-0.5f, -0.5f, 0.0f,
					 0.5f, -0.5f, 0.0f,
					 0.5f,  0.5f, 0.0f,
					-0.5f,  0.5f, 0.0f,
				},
				graphics::buffer::Layout{
					graphics::buffer::Element{{
							.name = "a_Position",
							.type = graphics::shader::data::Type::Float3
						}},
				}
			);

		auto square_index_buffer = Index_Buffer{};
		square_index_buffer.setup({0, 1, 2, 2, 3, 0});

		square_vertex_array.setup(std::move(square_vertex_buffer), std::move(square_index_buffer));

		renderer.set_clear_color({0.2f, 0.2f, 0.2f, 0.2f});

		square_shader.setup(
				R"(
					#version 330 core

					layout(location = 0) in vec3 a_Position;

					uniform mat4 u_ViewProjection;
					uniform mat4 u_Transform;

					out vec3 v_Position;

					void main() {
						v_Position = a_Position;
						gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
					}
				)",
				R"(
					#version 330 core

					layout(location = 0) out vec4 color;

					in vec3 v_Position;

					void main() {
						color = vec4(v_Position, 1.0);
					}
				)"
			);

		SAGE_LOG_DEBUG(*this);
	}

	auto teardown() -> void {
		layers.teardown();
		window.teardown();
		vertex_array.teardown();
		square_vertex_array.teardown();
		shader.teardown();
	}

public:
	friend REPR_DEF_FMT(App<Window, Input, Vertex_Array, Vertex_Buffer, Index_Buffer, Renderer, Shader, Ls...>)
	friend FMT_FORMATTER(App<Window, Input, Vertex_Array, Vertex_Buffer, Index_Buffer, Renderer, Shader, Ls...>);
};

}// sage

template <typename... Ts>
FMT_FORMATTER(sage::App<Ts...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::App<Ts...>) {
		return fmt::format_to(ctx.out(), "App:\n\twindow={}\n\tcamera={}\n\tlayers={}\n\t;", obj.window, obj.camera, obj.layers);
	}
};

