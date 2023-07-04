#pragma once

#include "src/graphics.hpp"

#include "glm/gtc/type_ptr.hpp"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

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

struct Shader {

private:
	uint32_t renderer_id = 0;

public:
	auto setup(const std::string& vertex_src, const std::string& fragment_src) -> void {
		SAGE_ASSERT(not renderer_id);

		// https://www.khronos.org/opengl/wiki/Shader_Compilation
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar *source = (const GLchar *)vertex_src.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			SAGE_ASSERT_MSG(false, fmt::format("Vertex shader compilation failed with: {}", infoLog.data()));
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar *)fragment_src.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			SAGE_ASSERT_MSG(false, fmt::format("Vertex shader compilation failed with: {}", infoLog.data()));
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		GLuint program = glCreateProgram();
		renderer_id = program;

		// Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

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
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			SAGE_ASSERT_MSG(false, fmt::format("Program compilation failed with: {}", infoLog.data()));
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
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

	auto upload_uniform_mat4(const std::string& name, const glm::mat4& u) const -> void {
		const auto loc = glGetUniformLocation(renderer_id, name.c_str());
		SAGE_ASSERT(loc != -1);

		glUniformMatrix4fv(
				loc,
				1,
				GL_FALSE,
				glm::value_ptr(u)
			);
	}
};

using Renderer_Base = sage::graphics::renderer::Base<Shader, Vertex_Array, Vertex_Buffer, Index_Buffer>;
struct Renderer : Renderer_Base {

public:
	auto submit(const Shader& shader, const Vertex_Array& va) -> void {
		Renderer_Base::submit(shader, va, [&] {
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
