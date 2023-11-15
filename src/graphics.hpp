#pragma once

#include "std.hpp"

#include "math.hpp"

#include "camera.hpp"
#include "repr.hpp"

namespace sage::graphics {

template <typename T>
concept Context =
	requires (T t) {
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

#pragma message "TODO: Material system will differentiate set/upload_uniform?"
template <typename S>
concept Concept =
	requires (S s, const Uniform& uniform, const std::string& uniform_name) {
		{ s.bind() } -> std::same_as<void>;
		{ s.unbind() } -> std::same_as<void>;
		{ s.upload_uniform(uniform_name, uniform) } -> std::same_as<void>;
		{ s.set(uniform_name, uniform) } -> std::same_as<void>;
	}
	;

struct Null {
	auto bind() {}
	auto unbind() {}
	auto upload_uniform(const auto&, const auto&) {}
	auto set(const auto&, const auto&) {}
};

struct Base {
protected:
	std::string name;

protected:
	static auto generate_name(const shader::Source& src) -> std::string {
		return src.path
			.or_else([] {
					static auto i = 1ul;
					SAGE_ASSERT(i < std::numeric_limits<decltype(i)>::max(),
							"Managed to instantiate {} shaders, impressive...", i
						);
					return std::optional{fs::path{fmt::format("Anonymous_Shader_{}", i++)}};
				})
			.and_then([] (const auto& path){
					return std::optional{path.stem()};
				})
			.value()
			.string()
			;
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
	Layout(Vector&& es)
		: _elements{std::move(es)}
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

// Quad fields must match the layout
struct Quad {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 tex_coord;
	// id, mask, ...

	static constexpr auto layout() -> Layout {
		return Layout{{
				buffer::Element{{ .name = "a_Position",	.type = shader::data::Type::Float3 }},
				buffer::Element{{ .name = "a_Color",	.type = shader::data::Type::Float4 }},
				buffer::Element{{ .name = "a_TexCoord", .type = shader::data::Type::Float2 }},
			}};
	}
};

template <typename VB>
concept Concept =
	requires (VB vb) {
		{ vb.bind() } -> std::same_as<void>;
		{ vb.unbind() } -> std::same_as<void>;
		{ vb.vertices() } -> std::same_as<const Vertices&>;
		{ vb.layout() } -> std::same_as<const Layout&>;
	}
	;

struct Null {
private:
	Vertices vert;
	Layout lay;
public:
	auto bind() {}
	auto unbind() {}
	auto vertices() -> const Vertices& { return vert; }
	auto layout() -> const Layout& { return lay; }
};

}//buffer::vertex

namespace index {

using Indeces = std::vector<uint32_t>;

template <typename IB>
concept Concept =
	requires (IB ib, Indeces&& indeces) {
		{ ib.bind() } -> std::same_as<void>;
		{ ib.unbind() } -> std::same_as<void>;
		{ ib.indeces() } -> std::same_as<const Indeces&>;
	}
	;

struct Null {
private:
	Indeces idxs;
public:
	auto bind() {}
	auto unbind() {}
	auto indeces() -> const Indeces& {
		return idxs;
	}
};

}//buffer::index

}// buffer

// This may change in the future since the idea of a "Vertex Array" is not
// really shared by rendering APIs besides OpenGL
namespace array::vertex {

template <typename A>
concept Concept =
	requires { typename A::Vertex_Buffer; } and buffer::vertex::Concept<typename A::Vertex_Buffer>
	and requires { typename A::Index_Buffer; } and buffer::index::Concept<typename A::Index_Buffer>
	and requires (A a) {
		{ a.bind() } -> std::same_as<void>;
		{ a.unbind() } -> std::same_as<void>;
	}
	;

struct Null {
	using Vertex_Buffer = buffer::vertex::Null;
	using Index_Buffer = buffer::index::Null;
	auto bind() {}
	auto unbind() {}
};

}// array::vertex

namespace texture {

template <typename T>
concept Concept =
	requires (T t, const fs::path& path, const size_t slot) {
		{ t.width() } -> std::same_as<size_t>;
		{ t.height() } -> std::same_as<size_t>;
		{ t.bind(slot) } -> std::same_as<void>;
		{ t.unbind() } -> std::same_as<void>;
	}
	;

}// texture

namespace renderer {

template <typename R, typename... Drawings>
concept Concept_2D =
	requires { typename R::Shader; } and shader::Concept<typename R::Shader>
	and requires { typename R::Vertex_Array; } and array::vertex::Concept<typename R::Vertex_Array>
	and requires { typename R::Draw_Args; }
	and requires (R r, const camera::Orthographic& cam, const std::function<void()>& draws, const Event& e) {
		{ r.scene(cam, draws) } -> std::same_as<void>;
		{ r.clear() } -> std::same_as<void>;
		{ r.event_callback(e) } -> std::same_as<void>;
	}
	and (sizeof...(Drawings) > 0)
	and (requires (R r, const Drawings& drawing, const typename R::Draw_Args& args) {
			{ r.draw(drawing, args) } -> std::same_as<void>;
		} and ...)
	;

struct Null {
	using Shader = shader::Null;
};

namespace detail {

template <typename Renderer, typename Drawings, size_t... Idxs>
consteval auto _renderer_can_draw(const std::index_sequence<Idxs...>&) -> bool {
	if constexpr (sizeof...(Idxs) > 0)
		return renderer::Concept_2D<Renderer, std::tuple_element_t<Idxs, Drawings>...>;
	else
		return true;
}

template <typename Renderer, typename Drawings>
constexpr auto renderer_can_draw = _renderer_can_draw<Renderer, Drawings>(std::make_index_sequence<std::tuple_size_v<Drawings>>{});

}// renderer::detail

// Convinence to pack together template information:
//
// template <Rendering Ring, typename... Stuff>
// struct App {
//		Ring::Renderer renderer;
// };
//
// struct Some_Rendering {
//		using Renderer = Linux_Renderer;
//		using Drawings = std::tuple<Linux_Texture, glm::vec4, int>;
// };
//
// using A = App<Some_Rendering, ...>
//
template <typename R>
concept Rendering =
		requires { typename R::Renderer; }
	and requires { typename R::Drawings; } // and std::same_as<typename R::Drawings, std::tuple<...>> // Im not gonna try to make this work syntactically...
	and detail::renderer_can_draw<typename R::Renderer, typename R::Drawings>
	;

struct Null_Rendering {
	using Renderer = renderer::Null;
	using Drawings = std::tuple<>;
};

template <typename _Vertex_Array, typename _Texture, typename _Shader>
	requires
			array::vertex::Concept<_Vertex_Array>
		and texture::Concept<_Texture>
		and shader::Concept<_Shader>
struct Base_2D {
protected:
	using Vertex_Array = _Vertex_Array;
	using Texture = _Texture;
	using Shader = _Shader;

protected:
	struct Scene_Data {
		Vertex_Array vertex_array;
		Shader shader;
	};

	struct Batch {
		using Vertices = std::vector<buffer::vertex::Quad>;

	public:
		static constexpr auto max_quads = 10'000;
		static constexpr auto max_vertices = max_quads * 4;
		static constexpr auto max_indeces = max_quads * 6;

	private:
		Vertices vertices;

	public:
		auto indexes() const -> size_t {
			return vertices.size() * 6;
		}

	public:
		friend struct Base_2D;	// A bit clunky but we simply want _just_ the Base to have direct access
	};

protected:
	Scene_Data scene_data;
	Batch batch;

private:
	bool scene_active = false;

private:
	const Texture default_texture = Texture{Size{1ul, 1ul}};

protected:
	Base_2D(Scene_Data&& sd)
		: scene_data{std::move(sd)}
	{
		batch.vertices.reserve(Batch::max_vertices);
	}

protected:
	// Should be called once, usually by some layer (see layer::Concept)
	//
	// struct Magic {
	//   Renderer& renderer;
	//   auto render() { renderer.draw(...); }
	// };
	//
	// struct Magic_Layer {
	//		Renderer& renderer;
	//   	Camera& camera;
	//   	Magic magic;
	//
	//   	auto render() {
	//	 	   renderer.scene(camera, [&] {
	//	 	   		magic.render();
	//	 	   	});
	//	 	}
	//	};
	//
	template <std::invocable Draws, std::invocable Flush>
	auto scene(const camera::Orthographic& cam, Draws&& draws, Flush&& impl) -> void {
		SAGE_ASSERT(not scene_active, "Must only call scene once: renderer.scene(camera, [] { render1(); render2(); });");

		scene_active = true;

		batch.vertices.clear();

		scene_data.shader.bind();
		scene_data.shader.set("u_ViewProjection", cam.view_proj_mat());

		draws();

		flush(std::forward<Flush>(impl));

		scene_active = false;
	}

	struct Draw_Args {
		const glm::vec3& position;
		const glm::vec2& size;
		float rotation = 0.f;
	};

	// Drawing is drawn with its center at `args.position` expanding outward.
	// Example (size width/height chose to make the code diagram legible)
	//
	// draw(color, { .position = { 5.f, 1.f }, .size = { 6.f, 4.f } }, ...);
	//
	//                |
	//                |  (2, -1)       (8, -1)
	//                |   |-----------|
	//  - - - - - - - + - + - - - - - + - - - - >
	//                |   |     o     |
	//                |   |           |
	//                |   |-----------|
	//                |  (2, 3)       (8, 3)
	//                |
	//                V
	//
	template <type::Any<Texture, glm::vec4> Drawing, std::invocable Impl>
	auto draw(const Drawing& drawing, const Draw_Args& args, Impl&& impl) {
		SAGE_ASSERT(scene_active);

		if constexpr (std::same_as<Drawing, Texture>) {
			drawing.bind();
			scene_data.shader.set("u_Color", glm::vec4{1.0f});
		}
		else if constexpr (std::same_as<Drawing, glm::vec4>) {
			default_texture.bind();

			// Batch a quad's 4 vertices
			batch.vertices.push_back({
					.position = { args.position.x - args.size.x / 2, args.position.y - args.size.y / 2, 0 },
					.color = drawing,
					.tex_coord = { 0.f, 0.f }
				});

			batch.vertices.push_back({
					.position = { args.position.x + args.size.x / 2, args.position.y - args.size.y / 2, 0 },
					.color = drawing,
					.tex_coord = { 1.f, 0.f }
				});

			batch.vertices.push_back({
					.position = { args.position.x + args.size.x / 2, args.position.y + args.size.y / 2, 0 },
					.color = drawing,
					.tex_coord = { 1.f, 1.f }
				});

			batch.vertices.push_back({
					.position = { args.position.x - args.size.x / 2, args.position.y + args.size.y / 2, 0 },
					.color = drawing,
					.tex_coord = { 0, 1.f }
				});
		}
		else
			static_assert(false, "Unhandled type");
	}

	template <std::invocable Impl>
	auto flush(Impl&& impl) -> void {
		const auto data = reinterpret_cast<const std::byte*>(batch.vertices.data());
		const auto bytes = batch.vertices.size() * sizeof(typename Batch::Vertices::value_type);

		scene_data.vertex_array.vertex_buffer()
			.set_vertices({data, bytes})
			;

		std::invoke(std::forward<Impl>(impl));
	}
};

template <typename R, typename Shader>
concept Concept =
	shader::Concept<Shader>
	and requires { typename R::Vertex_Array; } and array::vertex::Concept<typename R::Vertex_Array>
	and requires (R r, const camera::Orthographic& cam, const std::function<void()>& submissions, const Shader& shader, const glm::vec4& color, const typename R::Vertex_Array& va, const glm::mat4& transform, const Event& event) {
		{ r.event_callback(event) } -> std::same_as<void>;
		{ r.scene(cam, submissions) } -> std::same_as<void>;
		{ r.submit(shader, va, transform) } -> std::same_as<void>;
		{ r.clear() } -> std::same_as<void>;
		{ r.set_clear_color(color) } -> std::same_as<void>;
	}
	;

template <shader::Concept Shader, typename Vertex_Array>
	requires array::vertex::Concept<Vertex_Array>
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
		SAGE_ASSERT(scene_data.has_value(), "Renderer::submit() must be called in the submissions function passed to Renderer::scene()");

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
