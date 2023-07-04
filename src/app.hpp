#pragma once

#include "std.hpp"

#include "window.hpp"
#include "input.hpp"
#include "layer.hpp"
#include "layer_imgui.hpp"

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
		and graphics::renderer::Concept<Renderer, Vertex_Array, Vertex_Buffer, Index_Buffer>
		and (not same_as_any<layer::ImGui, Ls...>)	// The ImGui layer will always be provided by sage as the "overlay"
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

	Vertex_Array vertex_array, square_vertex_array;
	Renderer renderer;

	Shader shader, square_shader;

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

				SAGE_LOG_DEBUG(vertex_array);

				shader.setup(
					R"(
						#version 330 core

						layout(location = 0) in vec3 a_Position;
						layout(location = 1) in vec4 a_Color;

						out vec3 v_Position;
						out vec4 v_Color;

						void main() {
							v_Position = a_Position;
							v_Color = a_Color;
							gl_Position = vec4(a_Position, 1.0);
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
							-0.75f, -0.75f, 0.0f,
							 0.75f, -0.75f, 0.0f,
							 0.75f,  0.75f, 0.0f,
							-0.75f,  0.75f, 0.0f,
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

				SAGE_LOG_DEBUG(square_vertex_array);

				square_shader.setup(
						R"(
							#version 330 core

							layout(location = 0) in vec3 a_Position;

							out vec3 v_Position;

							void main() {
								v_Position = a_Position;
								gl_Position = vec4(a_Position, 1.0);
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

							// Update
				while (not stoken.stop_requested()) {
					renderer.set_clear_color({0.2f, 0.2f, 0.2f, 0.2f});
					renderer.clear();

					renderer.scene([&] {
							square_shader.bind();
							renderer.submit(square_vertex_array);

							shader.bind();
							renderer.submit(vertex_array);
						});

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
				vertex_array.teardown();
				square_vertex_array.teardown();
				shader.teardown();
			}};
	}
	auto stop() -> void {
		loop.request_stop();
		loop.join();
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
		return fmt::format_to(ctx.out(), "App:\n\twindow={}\n\tlayers={}\n\t;", obj.window, obj.layers);
	}
};

