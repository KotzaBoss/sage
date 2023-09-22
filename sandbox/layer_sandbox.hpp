#pragma once

#include "sage.hpp"

#include "platform/linux/input.hpp"
#include "platform/linux/graphics.hpp"

using namespace sage;

//struct Layer_Sandbox {
//	oslinux::Renderer& renderer;
//	oslinux::Vertex_Array vertex_array, square_vertex_array;
//	oslinux::Shader shader, square_shader, texture_shader;
//	glm::vec3 square_color;
//	oslinux::Texture2D tex;
//
//	using Camera_Controller = camera::Controller<oslinux::Input>;
//	Camera_Controller& camera_controller;
//
//	Layer_Sandbox(oslinux::Renderer& r, Camera_Controller& cc)
//		: renderer{r}
//		, camera_controller{cc}
//	{}
//
//	auto setup() -> void {
//		SAGE_LOG_DEBUG("SETUP");
//		auto vertex_buffer = oslinux::Vertex_Buffer{};
//		vertex_buffer.setup(
//				{
//					-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.f,
//					 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.f,
//					 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.f,
//				},
//				graphics::buffer::Layout{
//					graphics::buffer::Element{{
//							.name = "a_Position",
//							.type = graphics::shader::data::Type::Float3
//						}},
//					graphics::buffer::Element{{
//							.name = "a_Color",
//							.type = graphics::shader::data::Type::Float4
//					}},
//				}
//			);
//
//		auto index_buffer = oslinux::Index_Buffer{};
//		index_buffer.setup({0u, 1u, 2u});
//
//		vertex_array.setup(std::move(vertex_buffer), std::move(index_buffer));
//
//		shader.setup("asset/shader/some.glsl");
//
//		////////////////////////////////////////////////////////////////
//		////////////////////////////////////////////////////////////////
//		////////////////////////////////////////////////////////////////
//
//		auto square_vertex_buffer = oslinux::Vertex_Buffer{};
//		square_vertex_buffer.setup(
//				{
//					-0.5f, -0.5f, 0.0f, 0, 0,	// low left		black
//					 0.5f, -0.5f, 0.0f,	1, 0,	// low right	red
//					 0.5f,  0.5f, 0.0f,	1, 1,	// top right	yellow
//					-0.5f,  0.5f, 0.0f, 0, 1,	// top left		green
//				},
//				graphics::buffer::Layout{
//					graphics::buffer::Element{{
//							.name = "a_Position",
//							.type = graphics::shader::data::Type::Float3
//						}},
//					graphics::buffer::Element{{
//							.name = "a_TexCoord",
//							.type = graphics::shader::data::Type::Float2
//						}}
//				}
//			);
//
//		auto square_index_buffer = oslinux::Index_Buffer{};
//		square_index_buffer.setup({0, 1, 2, 2, 3, 0});
//
//		square_vertex_array.setup(std::move(square_vertex_buffer), std::move(square_index_buffer));
//
//		renderer.set_clear_color({0.5f, 0.5f, 0.5f, 1.f});
//		renderer.setup();
//
//		square_shader.setup("asset/shader/square.glsl");
//
//		texture_shader.setup("asset/shader/texture.glsl");
//		texture_shader.bind();
//		texture_shader.upload_uniform("u_Texture", 0);
//
//		tex.setup("asset/texture/owl.png");
//	}
//
//	auto update(const std::chrono::milliseconds delta) -> void {
//		renderer.clear();
//
//		renderer.scene(camera_controller.camera(), [&] {
//				const auto scale = glm::scale(glm::mat4{1}, glm::vec3{0.1f});
//				for (auto y = 0; y < 20; ++y) {
//					for (auto x = 0; x < 20; ++x) {
//						const auto transform = glm::translate(glm::mat4{1}, glm::vec3{x * 0.11f, y * 0.11f, 0}) * scale;
//
//						if (x % 2 and y % 2)
//							square_shader.upload_uniform("u_Color", square_color);
//						else
//							square_shader.upload_uniform("u_Color", glm::vec4{1, 1, 1, 1});
//
//						renderer.submit(square_shader, square_vertex_array, transform);
//					}
//				}
//
//				tex.bind();
//				renderer.submit(texture_shader, square_vertex_array, glm::scale(glm::mat4{1}, glm::vec3{1.5f}));
//			});
//
//		camera_controller.update(delta);
//	}
//
//	auto event_callback(const Event& e) -> void {
//		camera_controller.event_callback(e);
//	}
//
//	auto teardown() -> void {}
//
//	auto imgui_prepare() -> void {
//		ImGui::Begin("Settings");
//		ImGui::ColorEdit3("Square color", glm::value_ptr(square_color));
//		ImGui::End();
//	}
//
//};
//
//template <>
//FMT_FORMATTER(Layer_Sandbox) {
//	FMT_FORMATTER_DEFAULT_PARSE
//
//	FMT_FORMATTER_FORMAT(Layer_Sandbox) {
//		(void)obj;
//		return fmt::format_to(ctx.out(), "Layer_Sandbox;");
//	}
//};
//
