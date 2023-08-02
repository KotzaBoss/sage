#pragma once

#include "sage.hpp"

struct Layer_2D {
	using Camera_Controller = camera::Controller<oslinux::Input>;

public:
	Camera_Controller camera_controller;

	// Temp
	oslinux::Vertex_Array square_vertex_array;
	oslinux::Shader square_shader;

	oslinux::Renderer& renderer;

	glm::vec4 square_color = {0, 0, 0, 1};

public:
	Layer_2D(oslinux::Input& input, oslinux::Renderer& r)
		: camera_controller{input}
		, renderer{r}
	{}

public:
	auto setup() -> void {
		auto square_vertex_buffer = oslinux::Vertex_Buffer{};
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

		auto square_index_buffer = oslinux::Index_Buffer{};
		square_index_buffer.setup({0, 1, 2, 2, 3, 0});

		square_vertex_array.setup(std::move(square_vertex_buffer), std::move(square_index_buffer));

		renderer.set_clear_color({0.5f, 0.5f, 0.5f, 1.f});
		renderer.setup();

		square_shader.setup("asset/shader/flat_color.glsl");
	}

	auto update(const std::chrono::milliseconds delta) -> void {
		renderer.clear();

		renderer.scene(camera_controller.camera(), [&] {
				if (square_color.r < 1)
					square_color.r += 0.001f;

				square_shader.upload_uniform("u_Color", square_color);
				renderer.submit(square_shader, square_vertex_array, glm::scale(glm::mat4{1}, glm::vec3{1.5f}));
			});

		camera_controller.update(delta);
	}

	auto event_callback(const Event& e) -> void {
		camera_controller.event_callback(e);
	}

	auto teardown() -> void {}

	auto imgui_prepare() -> void {
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square color", glm::value_ptr(square_color));
		ImGui::End();
	}

};

template <>
FMT_FORMATTER(Layer_2D) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Layer_2D) {
		(void)obj;
		return fmt::format_to(ctx.out(), "Layer_2D;");
	}
};
