#pragma once

#include "std.hpp"

#include "glm/glm.hpp"

#include "camera.hpp"
#include "repr.hpp"

namespace sage::graphics {

template <typename T>
concept Context =
	requires (T t) {
		{ t.setup() } -> std::same_as<void>;
		{ t.swap_buffers() } -> std::same_as<void>;
	}
	;

namespace shader {

// FIXME: Is the uniform different from the shader::data::Types?
using Uniform = std::variant<
		int,
		float,
		glm::vec2,
		glm::vec3,
		glm::vec4,
		glm::mat3,
		glm::mat4
	>;

// Try to keep the values such that they can be used to index an array
// and don't forget to change MAX_SUPPORTED_TYPES if you add or remove shader::Types
enum class Type {
	None = -1 /* Try to cause trouble */,
	Vertex = 0, Fragment = 1,
};
constexpr auto MAX_SUPPORTED_TYPES = 2;

struct Source {
	std::string code;
	// Metadata
	std::optional<fs::path> path;
};

using Parsed = std::array<std::optional<shader::Source>, MAX_SUPPORTED_TYPES>;

template <typename S>
concept Concept =
	// requires s.setup(...)
	(
		requires (S s, const fs::path& src) {
			{ s.setup(src) } -> std::same_as<void>;
		}
		or requires (S s, const Parsed& shaders) {
			{ s.setup(shaders) } -> std::same_as<void>;
		}
	)
	and requires (S s, const Uniform& uniform, const std::string& uniform_name) {
		{ s.teardown() } -> std::same_as<void>;
		{ s.bind() } -> std::same_as<void>;
		{ s.unbind() } -> std::same_as<void>;
		{ s.upload_uniform(uniform_name, uniform) } -> std::same_as<void>;
	}
	;

struct Base {
protected:
	std::string name;

protected:
	static auto generate_name(const shader::Source& src) -> std::string {
		if (src.path.has_value())
			return src.path->stem().string();
		else {
			static auto i = 1ul;
			SAGE_ASSERT_MSG(i < std::numeric_limits<decltype(i)>::max(), "Managed to instantiate {} shaders, impressive...", i);
			return fmt::format("Anonymous_Shader_{}", i++);
		}
	}
};

namespace data {

enum class Type {
	None = 0,
	Bool,
	Int, Int2, Int3, Int4,
	Float, Float2, Float3, Float4,
	Mat3, Mat4,
};

inline auto size_of(const Type& t) -> size_t {
	switch (t) {
		case Type::Bool:	return 1;

		case Type::Int:		[[fallthrough]];
		case Type::Float:	return 4;

		case Type::Int2:	[[fallthrough]];
		case Type::Float2:	return 4 * 2;

		case Type::Int3:	[[fallthrough]];
		case Type::Float3:	return 4 * 3;

		case Type::Int4:	[[fallthrough]];
		case Type::Float4:	return 4 * 4;

		case Type::Mat3:	return 4 * 3 * 3;
		case Type::Mat4:	return 4 * 4 * 4;

		default:
			SAGE_ASSERT(false);
			return 0;
	}
}

inline auto component_count_of(const Type& t) -> size_t {
	switch (t) {
		case Type::Bool:	[[fallthrough]];
		case Type::Int:		[[fallthrough]];
		case Type::Float:	return 1;

		case Type::Int2:	[[fallthrough]];
		case Type::Float2:	return 2;

		case Type::Int3:	[[fallthrough]];
		case Type::Float3:	return 3;

		case Type::Int4:	[[fallthrough]];
		case Type::Float4:	return 4;

		case Type::Mat3:	return 3 * 3;
		case Type::Mat4:	return 4 * 4;

		default:
			SAGE_ASSERT(false);
			return 0;
	}
}

template <typename T>
concept Concept =
	requires (T t) {
		{ t.setup() } -> std::same_as<void>;
		{ t.teardown() } -> std::same_as<void>;
		{ t.bind() } -> std::same_as<void>;
		{ t.unbind() } -> std::same_as<void>;
	}
	;

}//shader::data

}//shader

namespace buffer {

struct Element {
	std::string name;
	shader::data::Type type;
	bool normalized;
	size_t size,
		   component_count,
		   offset;	// To be set manually later, correctly through a layout

public:
	struct Element_Args {
		std::string&& name; shader::data::Type type; bool normalized = false;
	};
	Element(Element_Args&& args)
		: name{std::move(args.name)}
		, type{args.type}
		, normalized{args.normalized}
		, size{shader::data::size_of(type)}
		, component_count{shader::data::component_count_of(type)}
		, offset{0}
	{}

public:
	REPR_DECL(Element);
};

struct Layout {
	using Vector = std::vector<Element>;

private:
	Vector _elements;
	size_t _stride;

public:
	Layout(std::same_as<Element> auto&&... es)
		: _elements{std::move(es)...}
		, _stride{0}
	{
		rg::for_each(_elements, [this, offset = 0ul] (auto& elem) mutable {
				elem.offset = offset;
				offset += elem.size;
				_stride += elem.size;
			});
	}

public:
	auto stride() const -> size_t {
		return _stride;
	}

	auto elements() const -> const Vector& {
		return _elements;
	}

public:
	REPR_DECL(Layout);
};

namespace vertex {

using Vertices = std::vector<float>;

template <typename VB>
concept Concept =
	requires (VB vb, Vertices&& vertices, buffer::Layout&& layout) {
		{ vb.setup(std::move(vertices), std::move(layout)) } -> std::same_as<void>;
		{ vb.teardown() } -> std::same_as<void>;
		{ vb.bind() } -> std::same_as<void>;
		{ vb.unbind() } -> std::same_as<void>;
	}
	;

}//buffer::vertex

namespace index {

using Indeces = std::vector<uint32_t>;

template <typename IB>
concept Concept =
	requires (IB ib, Indeces&& indeces) {
		{ ib.setup(std::move(indeces)) } -> std::same_as<void>;
		{ ib.teardown() } -> std::same_as<void>;
		{ ib.bind() } -> std::same_as<void>;
		{ ib.unbind() } -> std::same_as<void>;
		{ ib.indeces() } -> std::same_as<const Indeces&>;
	}
	;

}//buffer::index

}// buffer

// This may change in the future since the idea of a "Vertex Array" is not
// really shared by rendering APIs besides OpenGL
namespace array::vertex {

template <typename A, typename Vertex_Buffer, typename Index_Buffer>
concept Concept =
	buffer::vertex::Concept<Vertex_Buffer>
	and buffer::index::Concept<Index_Buffer>
	and requires (A a, Vertex_Buffer&& vb, Index_Buffer&& ib) {
		requires std::same_as<typename A::Vertex_Buffer, Vertex_Buffer>;
		requires std::same_as<typename A::Index_Buffer, Index_Buffer>;
		{ a.setup(std::move(vb), std::move(ib)) } -> std::same_as<void>;
		{ a.teardown() } -> std::same_as<void>;
		{ a.bind() } -> std::same_as<void>;
		{ a.unbind() } -> std::same_as<void>;
	}
	;

}// array::vertex

namespace texture {

template <typename T>
concept Concept =
	requires (T t, const fs::path& path, const size_t slot) {
		{ t.setup(path) } -> std::same_as<void>;
		{ t.teardown() } -> std::same_as<void>;
		{ t.width() } -> std::same_as<size_t>;
		{ t.height() } -> std::same_as<size_t>;
		{ t.bind(slot) } -> std::same_as<void>;
		{ t.unbind() } -> std::same_as<void>;
	}
	;

}// texture

namespace renderer {

template <typename R, typename Vertex_Array, typename Vertex_Buffer, typename Index_Buffer, typename Shader>
concept Concept =
	shader::Concept<Shader>
	and array::vertex::Concept<Vertex_Array, Vertex_Buffer, Index_Buffer>
	and requires (R r, const camera::Orthographic& cam, const std::function<void()>& submissions, const Shader& shader, const glm::vec4& color, const Vertex_Array& va, const glm::mat4& transform, const Event& event) {
		{ r.setup() } -> std::same_as<void>;
		{ r.event_callback(event) } -> std::same_as<void>;
		{ r.scene(cam, submissions) } -> std::same_as<void>;
		{ r.submit(shader, va, transform) } -> std::same_as<void>;
		{ r.clear() } -> std::same_as<void>;
		{ r.set_clear_color(color) } -> std::same_as<void>;
	}
	;

template <shader::Concept Shader, typename Vertex_Array, typename Vertex_Buffer, typename Index_Buffer>
	requires array::vertex::Concept<Vertex_Array, Vertex_Buffer, Index_Buffer>
struct Base {
	struct Scene_Data {
		glm::mat4 view_proj_mat;
	};

protected:
	std::optional<Scene_Data> scene_data;

public:
	auto scene(const camera::Orthographic& cam, const std::function<void()>& submissions) -> void {
		SAGE_ASSERT(not scene_data.has_value());

		scene_data = Scene_Data{};
		scene_data->view_proj_mat = cam.view_proj_mat();

		submissions();

		scene_data = std::nullopt;
	}

protected:	// Does not count as part of the concept just shares the same name to make the mental link, hence protected
	// Usage:
	//
	// struct OpenGL_Renderer : sage::graphics::renderer::Base {
	//
	//		auto submit(const Shader& shader, const Vertex_Array& va, const glm::mat4& transform) -> void {
	// 			Renderer_Base::submit(
	//					// Forward the "stuff" we need to render
	//					shader, va, transform,
	//					// Wrap the specifics of the draw call
	//					[&] {
	//						glDrawElements(GL_TRIANGLES, va.index_buffer().indeces().size(), GL_UNSIGNED_INT, nullptr);
	//					}
	// 			);
	// 		}
	//
	// };
	auto submit(const Shader& shader, const Vertex_Array& va, const glm::mat4& transform, const std::function<void()>& impl) -> void {
		SAGE_ASSERT(not va.vertex_buffer().layout().elements().empty());
		SAGE_ASSERT(not va.index_buffer().indeces().empty());
		SAGE_ASSERT_MSG(scene_data.has_value(), "Renderer::submit() must be called in the submissions function passed to Renderer::scene()");

		shader.bind();
		shader.upload_uniform("u_ViewProjection", scene_data->view_proj_mat);
		shader.upload_uniform("u_Transform", transform);
		va.bind();

		impl();
	}
};

}// renderer

}// sage::graphics

template <>
FMT_FORMATTER(sage::graphics::shader::data::Type) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::graphics::shader::data::Type) {
		using namespace sage::graphics::shader::data;

		return fmt::format_to(ctx.out(), "shader::Type: {:8};",
				std::invoke([&] { switch (obj) {
						case Type::Bool:	return "Bool";
						case Type::Int:		return "Int";
						case Type::Float:	return "Float";
						case Type::Int2:	return "Int2";
						case Type::Float2:	return "Float2";
						case Type::Int3:	return "Int3";
						case Type::Float3:	return "Float3";
						case Type::Int4:	return "Int4";
						case Type::Float4:	return "Float4";
						case Type::Mat3:	return "Mat3";
						case Type::Mat4:	return "Mat4";
						default:
							return "BAD";
					}})
			);
	}
};


template <>
FMT_FORMATTER(sage::graphics::buffer::Element) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::graphics::buffer::Element) {
		return fmt::format_to(ctx.out(), "Element: name={:20?}; type={} normalized={}; size={:3}; component_count={:3}; offset={:4};",
				obj.name, obj.type, obj.normalized, obj.size, obj.component_count, obj.offset
			);
	}
};

template <>
FMT_FORMATTER(sage::graphics::buffer::Layout) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::graphics::buffer::Layout) {
		fmt::format_to(ctx.out(), "Layout:\n\t\tstride={};\n", obj.stride());
		rg::for_each(obj.elements(), [&] (const auto& elem) {
				fmt::format_to(ctx.out(), "\t\t{};\n", elem);
			});
		return fmt::format_to(ctx.out(), "\t;");
	}
};

template <>
FMT_FORMATTER(sage::graphics::shader::Type) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::graphics::shader::Type) {
		using namespace sage::graphics::shader;

		return fmt::format_to(ctx.out(), "shader::Type: {};",
				std::invoke([&] { switch (obj) {
					case Type::Vertex:		return "Vertex";
					case Type::Fragment:	return "Fragment";
					default:				return "BAD VERTEX";
				}})
			);
	}
};

template <>
FMT_FORMATTER(sage::graphics::shader::Source) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::graphics::shader::Source) {
		fmt::format_to(ctx.out(), "shader::Source:\n\tpath={};\n\tcode=\n", obj.path);

		// Print code with line numbers
		auto line = std::string{};
		auto i = 1ul;
		for (auto istr = std::istringstream{obj.code}; std::getline(istr, line); )
			fmt::format_to(ctx.out(), "{:3}\t{}\n", i++, line);

		return fmt::format_to(ctx.out(), "\n\t;");
	}
};
