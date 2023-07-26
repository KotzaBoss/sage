#pragma once

#include "src/graphics.hpp"
#include "src/filesystem.hpp"

#include "glm/gtc/type_ptr.hpp"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "stb_image.h"

namespace sage::oslinux::inline graphics {

inline auto shader_data_type_to_opengl(const sage::graphics::shader::data::Type& t)  -> auto{
	using namespace sage::graphics::shader::data;

	switch (t) {
		case Type::Bool:	return GL_BOOL;

		case Type::Int:		[[fallthrough]];
		case Type::Int2:	[[fallthrough]];
		case Type::Int3:	[[fallthrough]];
		case Type::Int4:	return GL_INT;

		case Type::Float:	[[fallthrough]];
		case Type::Float2:	[[fallthrough]];
		case Type::Float3:	[[fallthrough]];
		case Type::Float4:	[[fallthrough]];
		case Type::Mat3:	[[fallthrough]];
		case Type::Mat4:	return GL_FLOAT;

		default:
			SAGE_ASSERT(false);
			return 0;
	}
}


struct OpenGL_Context {
private:
	GLFWwindow** glfw;

public:
	OpenGL_Context(GLFWwindow** w)
		: glfw{w}
	{
		SAGE_ASSERT(glfw);
	}

public:
	auto setup() -> void {
		glfwMakeContextCurrent(*glfw);

		const auto version = gladLoadGL(glfwGetProcAddress);
		SAGE_ASSERT(version);

		SAGE_LOG_INFO(R"end(OpenGL
				Version:	{}
				GLFW:		{}
				GLAD:		{}.{}
				Vendor:		{}
				Renderer:	{})end",
				(const char*)glGetString(GL_VERSION),
				glfwGetVersionString(),
				GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version),
				(const char*)glGetString(GL_VENDOR),
				(const char*)glGetString(GL_RENDERER)
			);
	}

	auto swap_buffers() -> void {
		glfwSwapBuffers(*glfw);
	}
};

struct Vertex_Buffer {
	using Layout = sage::graphics::buffer::Layout;
	using Vertices = sage::graphics::buffer::vertex::Vertices;

private:
	uint32_t renderer_id;
	Vertices _vertices;
	Layout _layout;

public:
	auto setup(Vertices vertices, Layout&& layout) -> void{
		_vertices = std::move(vertices);

		glCreateBuffers(1, &renderer_id);
		glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
		glBufferData(
				GL_ARRAY_BUFFER,
				_vertices.size() * sizeof(Vertices::value_type),
				_vertices.data(),
				GL_STATIC_DRAW
			);

		_layout = std::move(layout);
	}

	auto teardown() -> void {
		if (renderer_id)
			glDeleteBuffers(1, &renderer_id);
	}

	auto bind() const -> void {
		SAGE_ASSERT(renderer_id);
		glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
	}

	auto unbind() const -> void {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

public:
	auto vertices() const -> const Vertices& {
		return _vertices;
	}

	auto layout() const -> const Layout& {
		return _layout;
	}

public:
	REPR_DECL(Vertex_Buffer);
	friend FMT_FORMATTER(Vertex_Buffer);
};

struct Index_Buffer {
	using Indeces = sage::graphics::buffer::index::Indeces;

private:
	uint32_t renderer_id;
	size_t _size = 0;
	Indeces _indeces;

public:
	auto setup(Indeces&& indeces) -> void {
		_indeces = std::move(indeces);

		glCreateBuffers(1, &renderer_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id);
		glBufferData(
				GL_ELEMENT_ARRAY_BUFFER,
				_indeces.size() * sizeof(Indeces::value_type),
				_indeces.data(),
				GL_STATIC_DRAW
			);
	}

	auto teardown() -> void {
		if (renderer_id)
			glDeleteBuffers(1, &renderer_id);
	}

	auto indeces() const -> const Indeces& {
		return _indeces;
	}

public:
	auto bind() const -> void {
		SAGE_ASSERT(renderer_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id);
	}

	auto unbind() const -> void {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

public:
	friend FMT_FORMATTER(Index_Buffer);
};

struct Vertex_Array {
	using Vertex_Buffer = oslinux::Vertex_Buffer;
	using Index_Buffer = oslinux::Index_Buffer;

private:
	uint32_t renderer_id = 0;
	Vertex_Buffer _vertex_buffer;
	Index_Buffer _index_buffer;

public:
	auto setup(Vertex_Buffer&& vb, Index_Buffer&& ib) -> void {
		SAGE_ASSERT(not renderer_id);
		//SAGE_ASSERT(vb and ib);
		SAGE_ASSERT_MSG(vb.layout().elements().size(), "Vertex_Buffer has not been setup() yet");
		SAGE_ASSERT_MSG(ib.indeces().size(), "Index_Buffer has not been setup() yet");

		glCreateVertexArrays(1, &renderer_id);
		glBindVertexArray(renderer_id);

		_vertex_buffer = std::move(vb);
		_vertex_buffer.bind();

		rg::for_each(_vertex_buffer.layout().elements(), [&, index = 0ul] (const auto& elem) mutable {
				glEnableVertexAttribArray(index);
				glVertexAttribPointer(
						index,
						elem.component_count,
						shader_data_type_to_opengl(elem.type),
						elem.normalized ? GL_TRUE : GL_FALSE,
						_vertex_buffer.layout().stride(),
						(const void*)elem.offset
					);
				++index;
			});


		_index_buffer = std::move(ib);
		_index_buffer.bind();
	}

	auto teardown() -> void {
		if (renderer_id) {
			_vertex_buffer.teardown();
			_index_buffer.teardown();
			glDeleteVertexArrays(1, &renderer_id);
		}
	}

	auto bind() const -> void {
		SAGE_ASSERT(renderer_id);
		glBindVertexArray(renderer_id);
	}

	auto unbind() const -> void {
		glBindVertexArray(renderer_id);
	}

public:
	auto vertex_buffer() const -> const Vertex_Buffer& {
		return _vertex_buffer;
	}

	auto index_buffer() const -> const Index_Buffer& {
		return _index_buffer;
	}

public:
	friend FMT_FORMATTER(Vertex_Array);
};

// When setting up the shader from file do not try to manually read the contents of the file
// and pass them to the setup(string, string) method. Instead use the setup(fs::path) overload.
//
// See those methods for details.
struct Shader {
	using Parsed_Shaders = sage::graphics::shader::Parsed;

	static constexpr int shader_type_map[] = {
			[std::to_underlying(sage::graphics::shader::Type::Vertex)] = GL_VERTEX_SHADER,
			[std::to_underlying(sage::graphics::shader::Type::Fragment)] = GL_FRAGMENT_SHADER,
		};

private:
	uint32_t renderer_id = 0;

public:
	// The shader file sources are expected to have at least one line at the top of the file:
	// #type XXX
	// where XXX is the supported shader type (generally: vertex, fragment).
	// Subsequent shaders are delimitated by another "#type XXX" line.
	// Empty lines do not matter.
	//
	// For details on the parsing of the shader sources see parse_shaders() method.
	auto setup(const fs::path& p) -> void {
		setup(parse_shaders(sage::read_file(p), p));
	}

	auto setup(const Parsed_Shaders& shaders) -> void {
		using namespace sage::graphics;

		SAGE_ASSERT(not renderer_id);

		// FIXME: why shader.size() doesnt work? its an array
		auto processed_shaders = std::array<GLuint, shader::MAX_SUPPORTED_TYPES>{};

		rg::for_each(shaders, [&, this, i = 0 /* Poor man's enumerate */] (const auto& source) mutable {
				if (source.has_value()) {
					// https://www.khronos.org/opengl/wiki/Shader_Compilation

					// Create an empty shader handle
					GLuint shader_id = glCreateShader(shader_type_map[i]);

					// Send the shader source code to GL
					// Note that std::string's .c_str is NULL character terminated.
					const GLchar *source_ptr = (const GLchar *)source->code.c_str();
					glShaderSource(shader_id, 1, &source_ptr, 0);

					// Compile the vertex shader
					glCompileShader(shader_id);

					GLint isCompiled = 0;
					glGetShaderiv(shader_id, GL_COMPILE_STATUS, &isCompiled);
					if(isCompiled == GL_FALSE)
					{
						GLint maxLength = 0;
						glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &maxLength);

						// The maxLength includes the NULL character
						std::vector<GLchar> infoLog(maxLength);
						glGetShaderInfoLog(shader_id, maxLength, &maxLength, &infoLog[0]);

						// We don't need the shader anymore.
						glDeleteShader(shader_id);

						SAGE_ASSERT_MSG(false, "{} Compilation failed with: {}\n{}", static_cast<shader::Type>(i), infoLog.data(), *source);
					}
					else
						processed_shaders[i] = shader_id;
				}

				++i;

			});

		SAGE_LOG_DEBUG("Compiled shaders ids (0 means failed to compile): {}", processed_shaders);
		SAGE_ASSERT(rg::all_of(processed_shaders, [] (const auto shader_id) { return shader_id != 0; }));

		// Shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		GLuint program = glCreateProgram();

		// Attach our shaders to our program
		rg::for_each(processed_shaders, [=] (const auto shader) {
				glAttachShader(program, shader);
			});

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			rg::for_each(processed_shaders, [=] (const auto shader) {
					glDeleteShader(shader);
				});

			SAGE_ASSERT_MSG(false, fmt::format("Program compilation failed with: {}", infoLog.data()));
			return;
		}

		// Always detach shaders after a successful link.
		rg::for_each(processed_shaders, [=] (const auto shader) {
				glDetachShader(program, shader);
			});

		renderer_id = program;
	}

	auto teardown() {
		SAGE_ASSERT(renderer_id);
		glDeleteProgram(renderer_id);
	}

	auto bind() const -> void {
		glUseProgram(renderer_id);
	}

	auto unbind() const -> void {
		glUseProgram(0);
	}

	auto upload_uniform(const std::string& name, const sage::graphics::shader::Uniform& uniform) const -> void {
		const auto loc = glGetUniformLocation(renderer_id, name.c_str());
		SAGE_ASSERT(loc != -1);

		std::visit(
				[&] (auto&& u) {
					using T = std::decay_t<decltype(u)>;
					if constexpr (std::same_as<int, T>)
						glUniform1i(loc,
								u
							);
					else if constexpr (std::same_as<float, T>)
						glUniform1f(loc,
								u
							);
					else if constexpr (std::same_as<glm::vec2, T>)
						glUniform2f(loc,
								u.x, u.y
							);
					else if constexpr (std::same_as<glm::vec3, T>)
						glUniform3f(loc,
								u.x, u.y, u.z
							);
					else if constexpr (std::same_as<glm::vec4, T>)
						glUniform4f(loc,
								u.x, u.y, u.z, u.w
							);
					else if constexpr (std::same_as<glm::mat3, T>)
						glUniformMatrix3fv(loc,
								1,
								GL_FALSE,
								glm::value_ptr(u)
							);
					else if constexpr (std::same_as<glm::mat4, T>)
						glUniformMatrix4fv(loc,
								1,
								GL_FALSE,
								glm::value_ptr(u)
							);
					else {
						// static_assert(false);	// Why can i never get this to work correctly?
						SAGE_ASSERT_MSG(false, fmt::format("Uniform holds type of index {}, which is not supported", uniform.index()));
					}
				},
				uniform
			);
	}

private:

	static auto parse_shaders(const std::string& file_src, const std::optional<fs::path>& path = std::nullopt) -> sage::graphics::shader::Parsed {
		constexpr auto type_token = "#type "sv;		// Note the convenient space

		// Supported shaders
		constexpr auto supported_shaders = std::array{ "vertex", "fragment" };
		SAGE_ASSERT_MSG(file_src.find(type_token) != std::string::npos, "At least one #type of shader must exist. Supported: {}", supported_shaders);

		auto shaders = Parsed_Shaders{};

		// Process file_src
		{
			using namespace sage::graphics;

			auto line = std::string{};
			auto shader = shader::Type::None;
			for (auto istream = std::istringstream{file_src}; std::getline(istream, line); ) {
				string::trim(line);
				if (line.starts_with(type_token)) {
					const auto shader_i = rg::find_if(supported_shaders, [&] (const auto& shader) { return line.ends_with(shader); });
					SAGE_ASSERT(shader_i != supported_shaders.cend());
					shader = static_cast<shader::Type>(std::distance(supported_shaders.cbegin(), shader_i));	// Distance (index) should match the enum value
					shaders[std::to_underlying(shader)].emplace("", path);
				}
				else {
					SAGE_ASSERT_MSG(shader != shader::Type::None, "Make sure there that the first line of the file has some shader #type");

					auto& source = shaders[std::to_underlying(shader)];
					SAGE_ASSERT(source.has_value());

					source->code
						.append(std::move(line))
						.push_back('\n')
						;
				}
			}
		}

		return shaders;
	}
};

struct Texture2D {
private:
	fs::path path;
	size_t _width,
		   _height;
	uint32_t renderer_id = 0;

public:
	auto setup(const fs::path& p) -> void {
		SAGE_ASSERT_MSG(fs::exists(p), "Current: {}; Requested: {}", fs::current_path(), p);
		path = p;

		stbi_set_flip_vertically_on_load(1);

		int width, height, channels;
		const auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		SAGE_ASSERT_MSG(data, "stbi could not load from: {}", path.c_str());

		_width = width;
		_height = height;
		SAGE_ASSERT(channels == 3 or channels == 4);
		const auto internal_format = channels == 3 ? GL_RGB8 : GL_RGBA8;
		const auto data_format = channels == 3 ? GL_RGB : GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id);
		glTextureStorage2D(renderer_id, 1, internal_format, _width, _height);

		glTextureParameteri(renderer_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(renderer_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(renderer_id,
				0,
				0, 0,
				_width, _height,
				data_format,
				GL_UNSIGNED_BYTE,
				data
			);

		stbi_image_free(data);
	}

	auto teardown() -> void {
		glDeleteTextures(1, &renderer_id);
	}

public:
	auto width() const -> size_t { return _width; }
	auto height() const -> size_t { return _height; }

public:
	auto bind(const size_t slot = 0) const -> void {
		SAGE_ASSERT(renderer_id);
		glBindTextureUnit(slot, renderer_id);
	}
	auto unbind() const -> void {}
};

using Renderer_Base = sage::graphics::renderer::Base<Shader, Vertex_Array, Vertex_Buffer, Index_Buffer>;

struct Renderer : Renderer_Base {

public:
	auto setup() -> void {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	auto submit(const Shader& shader, const Vertex_Array& va, const glm::mat4& transform = glm::mat4{1.f}) -> void {
		Renderer_Base::submit(shader, va, transform, [&] {
				glDrawElements(GL_TRIANGLES, va.index_buffer().indeces().size(), GL_UNSIGNED_INT, nullptr);
			});
	}

	auto clear() -> void {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	auto set_clear_color(const glm::vec4& color) -> void {
		glClearColor(color.r, color.g, color.b, color.a);
	}
};

}//sage::oslinux::graphics

template <>
FMT_FORMATTER(sage::oslinux::Vertex_Buffer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::oslinux::Vertex_Buffer) {
		return fmt::format_to(ctx.out(), "oslinux::Vertex_Buffer: vertices={}; layout={};;", obj._vertices, obj._layout);
	}
};

template <>
FMT_FORMATTER(sage::oslinux::Index_Buffer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::oslinux::Index_Buffer) {
		return fmt::format_to(ctx.out(), "oslinux::Index_Buffer: size={} indeces={};", obj._indeces.size(), obj._indeces);
	}
};

template <>
FMT_FORMATTER(sage::oslinux::Vertex_Array) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::oslinux::Vertex_Array) {
		return fmt::format_to(ctx.out(), "oslinux::Vertex_Array: vertex_buffer={} index_buffer={};",
				obj._vertex_buffer, obj._index_buffer
			);
	}
};
