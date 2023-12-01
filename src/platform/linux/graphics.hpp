#pragma once

#include "src/graphics.hpp"
#include "src/filesystem.hpp"

#include "src/math.hpp"

#include "glad/gl.h"
#include "glfw.hpp"

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
	GLFWwindow* glfw;

public:
	OpenGL_Context(GLFWwindow* w)
		: glfw{w}
	{
		SAGE_ASSERT(glfw);
		glfwMakeContextCurrent(glfw);

		const auto version = gladLoadGL(glfwGetProcAddress);
		SAGE_ASSERT(version);

		// Thanks: https://www.khronos.org/opengl/wiki/Example/OpenGL_Error_Testing_with_Message_Callbacks
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(gl_error_callback, nullptr);

		const auto gl_version = std::string_view{reinterpret_cast<const char*>(glGetString(GL_VERSION))};

		SAGE_LOG_INFO(R"end(OpenGL
				Version:	{}
				GLFW:		{}
				GLAD:		{}.{}
				Vendor:		{}
				Renderer:	{})end",
				gl_version.data(),
				glfwGetVersionString(),
				GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version),
				(const char*)glGetString(GL_VENDOR),
				(const char*)glGetString(GL_RENDERER)
			);

		SAGE_ASSERT(gl_version.contains("4.6"));
	}

	OpenGL_Context(OpenGL_Context&& other)
		: glfw{std::exchange(other.glfw, nullptr)}
	{}

public:
	auto swap_buffers() -> void {
		glfwSwapBuffers(glfw);
	}

private:
	 static GLAPIENTRY auto gl_error_callback(
		GLenum source,
		GLenum type,
		[[maybe_unused]] GLuint id,
		[[maybe_unused]] GLenum severity,
		[[maybe_unused]] GLsizei length,
		const GLchar* message,
		[[maybe_unused]] const void* userParam
	) -> void
	{
		if (type == GL_DEBUG_TYPE_ERROR)
			SAGE_LOG_ERROR(
					"OpenGL Error:\n\t{} {}",
					source, message
				);
	}

};

// TODO: Split dynamic/static vertex_buffer?
struct Vertex_Buffer {
	using Layout = sage::graphics::buffer::Layout;
	using Vertices = sage::graphics::buffer::vertex::Vertices;

private:
	glfw::ID renderer_id;
	Vertices _vertices;
	Layout _layout;

public:
	Vertex_Buffer(const size_t size, Layout&& l)
		: _layout{std::move(l)}
	{
		renderer_id.emplace();
		glCreateBuffers(1, &renderer_id.value());
		glBindBuffer(GL_ARRAY_BUFFER, *renderer_id);
		glBufferData(
				GL_ARRAY_BUFFER,
				size,
				nullptr,
				GL_DYNAMIC_DRAW
			);
	}

	Vertex_Buffer(Vertices&& v, Layout&& l)
		: _vertices{std::move(v)}
		, _layout{std::move(l)}
	{
		renderer_id.emplace();
		glCreateBuffers(1, &renderer_id.value());
		glBindBuffer(GL_ARRAY_BUFFER, *renderer_id);
		glBufferData(
				GL_ARRAY_BUFFER,
				_vertices.size() * sizeof(Vertices::value_type),
				_vertices.data(),
				GL_STATIC_DRAW
			);
	}

	Vertex_Buffer(Vertex_Buffer&& other)
		: renderer_id{std::move(other.renderer_id)}
		, _vertices{std::move(other._vertices)}
		, _layout{std::move(other._layout)}
	{}

	~Vertex_Buffer() {
		if (renderer_id)
			glDeleteBuffers(1, &renderer_id.value());
	}

public:
	auto bind() const -> void {
		SAGE_ASSERT(*renderer_id);
		glBindBuffer(GL_ARRAY_BUFFER, *renderer_id);
	}

	auto unbind() const -> void {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

public:
	auto vertices() const -> const Vertices& {
		return _vertices;
	}

	auto set_vertices(const std::span<const std::byte> bytes) -> void {
		SAGE_ASSERT(*renderer_id);
		glBindBuffer(GL_ARRAY_BUFFER, *renderer_id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, bytes.size(), bytes.data());
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
	glfw::ID renderer_id;
	Indeces _indeces;

public:
	Index_Buffer(Indeces&& indeces)
		: _indeces{std::move(indeces)}
	{
		renderer_id.emplace();
		glCreateBuffers(1, &renderer_id.value());

		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
		// Thanks: TheCherno/Hazel
		glBindBuffer(GL_ARRAY_BUFFER, *renderer_id);
		glBufferData(
				GL_ARRAY_BUFFER,
				_indeces.size() * sizeof(Indeces::value_type),
				_indeces.data(),
				GL_STATIC_DRAW
			);

		if constexpr (build::release)
			_indeces.clear();
	}

	constexpr
	Index_Buffer(const size_t size)
		: Index_Buffer{std::invoke([&] {
				// TODO: This could be a range::chunk or range::slide in c++23?
				auto indeces = Indeces{};
				indeces.reserve(size);

				for (auto offset = 0ul, i = 0ul;
					i < indeces.capacity();
					offset += 4, i += 6)
				{
					const auto idxs = {
							offset + 0ul,
							offset + 1ul,
							offset + 2ul,

							offset + 2ul,
							offset + 3ul,
							offset + 0ul,
						};

					SAGE_ASSERT(indeces.end() + idxs.size() <= indeces.begin() + indeces.capacity(),
							"Writing past end of indeces buffer, Index_Buffer size must be divisible by {}", idxs.size());

					indeces.insert(indeces.end(), idxs.begin(), idxs.end());
				}

				return indeces;
			})}
	{}

	Index_Buffer(Index_Buffer&& other)
		: renderer_id{std::move(other.renderer_id)}
		, _indeces{std::move(other._indeces)}
	{}

	~Index_Buffer() {
		if (renderer_id)
			glDeleteBuffers(1, &renderer_id.value());
	}

public:
	auto indeces() const -> const Indeces& {
		return _indeces;
	}

public:
	auto bind() const -> void {
		SAGE_ASSERT(*renderer_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *renderer_id);
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
	glfw::ID renderer_id;
	Vertex_Buffer _vertex_buffer;
	Index_Buffer _index_buffer;

public:
	Vertex_Array(Vertex_Buffer&& vb, Index_Buffer&& ib)
		: _vertex_buffer{std::move(vb)}
		, _index_buffer{std::move(ib)}
	{
		SAGE_ASSERT(not renderer_id);
		SAGE_ASSERT(_vertex_buffer.layout().elements().size());
		SAGE_ASSERT(_index_buffer.indeces().size());

		renderer_id.emplace();
		glCreateVertexArrays(1, &renderer_id.value());

		glBindVertexArray(*renderer_id);

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


		_index_buffer.bind();
	}

	Vertex_Array(Vertex_Array&& other)
		: renderer_id{std::move(other.renderer_id)}
		, _vertex_buffer{std::move(other._vertex_buffer)}
		, _index_buffer{std::move(other._index_buffer)}
	{}

	~Vertex_Array() {
		if (renderer_id) {
			glDeleteVertexArrays(1, &renderer_id.value());
		}
	}

public:
	auto bind() const -> void {
		SAGE_ASSERT(renderer_id);
		glBindVertexArray(*renderer_id);
	}

	auto unbind() const -> void {
		glBindVertexArray(*renderer_id);
	}

public:
	auto vertex_buffer() const	-> const Vertex_Buffer&	{ return _vertex_buffer; }
	auto vertex_buffer()		-> Vertex_Buffer&		{ return _vertex_buffer; }

	auto index_buffer() const -> const Index_Buffer& {
		return _index_buffer;
	}

public:
	friend FMT_FORMATTER(Vertex_Array);
};

// When setting up the shader from file do not try to manually read the contents of the file
// and pass them to the Shader(string, string) constructor. Instead use the Shader(fs::path) overload.
//
// See those methods for details.
struct Shader : sage::graphics::shader::Base {
	using Base = sage::graphics::shader::Base;
	using Parsed_Shaders = sage::graphics::shader::Parsed;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	static constexpr int shader_type_map[] = {
			[std::to_underlying(sage::graphics::shader::Type::Vertex)] = GL_VERTEX_SHADER,
			[std::to_underlying(sage::graphics::shader::Type::Fragment)] = GL_FRAGMENT_SHADER,
		};
#pragma GCC diagnostic pop

private:
	glfw::ID renderer_id;

public:
	// The shader file sources are expected to have at least one line at the top of the file:
	// #type XXX
	// where XXX is the supported shader type (generally: vertex, fragment).
	// Subsequent shaders are delimitated by another "#type XXX" line.
	// Empty lines do not matter.
	//
	// For details on the parsing of the shader sources see parse_shaders() method.
	Shader(const fs::path& p)
		: Shader{parse_shaders(sage::read_file(p), p)}
	{}

	Shader(const Parsed_Shaders& shaders) {
		using namespace sage::graphics;

		SAGE_ASSERT(not renderer_id);

		// FIXME: why shader.size() doesnt work? its an array
		auto processed_shaders = std::array<GLuint, shader::MAX_SUPPORTED_TYPES>{};

		rg::for_each(shaders | vw::enumerate, [&, this] (const auto& i_source) mutable {
				const auto& [i, source] = i_source;
				if (source.has_value()) {
					this->name = Base::generate_name(*source);

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

						SAGE_ASSERT(false, "{} Compilation failed with: {}\n{}", static_cast<shader::Type>(i), infoLog.data(), *source);
					}
					else
						processed_shaders[i] = shader_id;
				}
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

			SAGE_ASSERT(false, "Program compilation failed with: {}", infoLog.data());
			return;
		}

		// Always detach shaders after a successful link.
		rg::for_each(processed_shaders, [=] (const auto shader) {
				glDetachShader(program, shader);
			});

		renderer_id = program;
	}

	Shader(Shader&& other)
		: renderer_id{std::move(other.renderer_id)}
	{}

	~Shader() {
		if (renderer_id)
			glDeleteProgram(*renderer_id);
	}

	auto bind() const -> void {
		glUseProgram(*renderer_id);
	}

	auto unbind() const -> void {
		glUseProgram(0);
	}

	auto upload_uniform(const std::string& name, const sage::graphics::shader::Uniform& uniform) const -> void {
		SAGE_ASSERT(renderer_id);
		glUseProgram(*renderer_id);
		const auto loc = glGetUniformLocation(*renderer_id, name.c_str());
		SAGE_ASSERT(loc != -1, "Cannot find uniform {:?}", name);

		std::visit(Overloaded {
					[&] (const int u)					{ glUniform1i (loc, u);										},
					[&] (const std::span<const int> v)	{ glUniform1iv(loc, v.size(), v.data() );					},
					[&] (const float u)					{ glUniform1f (loc, u);										},
					[&] (const glm::vec2& u)			{ glUniform2f (loc, u.x, u.y); 								},
					[&] (const glm::vec3& u) 			{ glUniform3f (loc, u.x, u.y, u.z);							},
					[&] (const glm::vec4& u) 			{ glUniform4f (loc, u.x, u.y, u.z, u.w );					},
					[&] (const glm::mat3& u) 			{ glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(u));	},
					[&] (const glm::mat4& u) 			{ glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(u));	},
					[&] <typename T> (T&& x)			{ SAGE_DIE("{}", type::real_name<T>());		}
				},
				uniform
			);
	}

	auto set(const std::string& name, const sage::graphics::shader::Uniform& value) const -> void {
		upload_uniform(name, value);
	}

private:

	static auto parse_shaders(const std::string& file_src, const std::optional<fs::path>& path = std::nullopt) -> sage::graphics::shader::Parsed {
		constexpr auto type_token = "#type "sv;		// Note the convenient space

		// Supported shaders
		constexpr auto supported_shaders = std::array{ "vertex", "fragment" };
		SAGE_ASSERT(file_src.find(type_token) != std::string::npos, "At least one #type of shader must exist. Supported: {}", supported_shaders);

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
					SAGE_ASSERT(shader != shader::Type::None, "Make sure there that the first line of the file has some shader #type");

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
	using Size = sage::math::Size<size_t>;

private:
	fs::path path;
	Size size;
	GLenum internal_format,
		   data_format;
	size_t channels;
	glfw::ID renderer_id;

public:
	Texture2D(const Size& sz, const size_t channels = 4)
		: size{sz}
		, channels{channels}
	{
		SAGE_ASSERT(channels == 3 or channels == 4);

		internal_format = channels == 3 ? GL_RGB8 : GL_RGBA8;
		data_format = channels == 3 ? GL_RGB : GL_RGBA;

		renderer_id.emplace();
		glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id.value());
		glTextureStorage2D(*renderer_id, 1, internal_format, size.width, size.height);

		glTextureParameteri(*renderer_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(*renderer_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(*renderer_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(*renderer_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// OPTIMIZE: ?
		const auto data = std::vector(size.width * size.height * channels, std::byte{0xff});
		set_data(data);
	}

	Texture2D(const fs::path& p) {
		SAGE_ASSERT_PATH_EXISTS(p);
		path = p;

		stbi_set_flip_vertically_on_load(1);

		int w, h, chan;
		const auto data = stbi_load(path.c_str(), &w, &h, &chan, 0);
		SAGE_ASSERT(data, "stbi could not load from: {}", path.c_str());
		SAGE_ASSERT(chan == 3 or chan == 4);

		size.width = w;
		size.height = h;
		channels = chan;

		internal_format = channels == 3 ? GL_RGB8 : GL_RGBA8;
		data_format = channels == 3 ? GL_RGB : GL_RGBA;

		renderer_id.emplace();
		glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id.value());
		glTextureStorage2D(*renderer_id, 1, internal_format, size.width, size.height);

		glTextureParameteri(*renderer_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(*renderer_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(*renderer_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(*renderer_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(*renderer_id,
				0,
				0, 0,
				size.width, size.height,
				data_format,
				GL_UNSIGNED_BYTE,
				data
			);

		stbi_image_free(data);
	}

	Texture2D(Texture2D&& other)
		: path{std::move(other.path)}
		, size{other.size}
		, renderer_id{std::move(other.renderer_id)}
	{}

	~Texture2D() {
		if (renderer_id)
			glDeleteTextures(1, &renderer_id.value());
	}

public:
	auto width() const -> size_t { return size.width; }
	auto height() const -> size_t { return size.height; }

public:
	auto set_data(const std::span<const std::byte> data) -> void {
		SAGE_ASSERT(renderer_id);
		SAGE_ASSERT(data.size() == size.width * size.height * channels,
				"Data must fill the entire texture");

		glTextureSubImage2D(
				*renderer_id,
				0,
				0, 0,
				size.width, size.height,
				data_format,
				GL_UNSIGNED_BYTE,
				data.data()
			);
	}

	auto bind(const size_t slot = 0) const -> void {
		SAGE_ASSERT(renderer_id);
		glBindTextureUnit(slot, *renderer_id);
	}
	auto unbind() const -> void {}

	auto operator== (const Texture2D& other) const -> bool {
		return renderer_id == other.renderer_id;
	}
};

struct Renderer_2D : sage::graphics::renderer::Base_2D<Vertex_Array, Texture2D, Shader> {
	using Base = sage::graphics::renderer::Base_2D<Vertex_Array, Texture2D, Shader>;
	using Batch = Base::Batch;
	using Vertex_Array = Base::Vertex_Array;
	using Shader = Base::Shader;
	using Draw_Args = Base::Draw_Args;

public:
	Renderer_2D()
		: Base{{
			.vertex_array{
				Vertex_Buffer{
					Batch::max_quads * sizeof(sage::graphics::buffer::vertex::Quad),
					sage::graphics::buffer::vertex::Quad::layout()
				},
				Index_Buffer{Batch::max_indeces}
			},
			.shader{"asset/shader/texture.glsl"}
		}}
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.5f, 0.5f, 0.5f, 1.f);

		scene_data.shader.bind();
		// Make an iota array to fill the Sampler2Ds
		auto iota = std::array<int, Base::Batch::max_texture_slots>{};
		rg::iota(iota, 0);
		scene_data.shader.upload_uniform("u_Textures", std::span{iota});
	}

	template <std::invocable Draws>
	auto scene(const camera::Orthographic& cam, Draws&& draws) -> void {
		Base::scene(cam, std::forward<Draws>(draws), [this] {
				glDrawElements(GL_TRIANGLES, batch.indexes(), GL_UNSIGNED_INT, nullptr);
			});
	}

	template <type::Any<Texture2D, glm::vec4> Drawing>
	auto draw(const Drawing& drawing, const Base::Draw_Args& args) {
		Base::draw(drawing, args);
	}

	auto clear() -> void {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	auto event_callback(const Event& e) -> void {
		if (e.type == Event::Type::Window_Resized) {
			SAGE_ASSERT(std::holds_alternative<Size<size_t>>(e.payload));

			const auto& payload = std::get<Size<size_t>>(e.payload);
			glViewport(0, 0, payload.width, payload.height);
		}
	}
};

using Renderer_Base = sage::graphics::renderer::Base<Shader, oslinux::Vertex_Array>;
struct Renderer : Renderer_Base {
	using Vertex_Array = oslinux::Vertex_Array;

public:
	Renderer() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
	}

	auto event_callback(const Event& e) -> void {
		if (e.type == Event::Type::Window_Resized) {
			SAGE_ASSERT(std::holds_alternative<Size<size_t>>(e.payload));

			const auto& payload = std::get<Size<size_t>>(e.payload);
			glViewport(0, 0, payload.width, payload.height);
		}
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
