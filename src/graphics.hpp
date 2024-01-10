#pragma once

#include "src/std.hpp"

#include "src/math.hpp"
#include "src/util.hpp"
#include "src/perf.hpp"

#include "src/camera.hpp"
#include "src/repr.hpp"

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
		std::span<const int>,
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

// TODO: Material system will differentiate set/upload_uniform?
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
	struct Texture {
		glm::vec2 coord;
		float index = 0;	// Not sure why uints do not work...
	} texture;

	// id, mask, ...

	static constexpr auto layout() -> Layout {
		return Layout{{
				buffer::Element{{ .name = "a_Position",	.type = shader::data::Type::Float3	}},
				buffer::Element{{ .name = "a_Color",	.type = shader::data::Type::Float4	}},
				buffer::Element{{ .name = "a_TexCoord", .type = shader::data::Type::Float2	}},
				buffer::Element{{ .name = "a_TexIndex",	.type = shader::data::Type::Float	}},
			}};
	}
};

template <typename VB>
concept Concept =
	requires (VB vb) {
		{ vb.bind() } -> std::same_as<void>;
		{ vb.unbind() } -> std::same_as<void>;
		{ vb.verteces() } -> std::same_as<const Vertices&>;
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
	auto verteces() -> const Vertices& { return vert; }
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

namespace frame {

struct Attrs {
	glm::vec2 size;
	size_t samples = 1;
	bool is_swap_chain_target = false;
};

template <typename FB>
concept Concept = requires(FB fb, const glm::vec2& new_size) {
		{ fb.bind() } -> std::same_as<void>;
		{ fb.unbind() } -> std::same_as<void>;
		{ fb.color_attachment_id() } -> std::convertible_to<void*>;
		{ fb.resize(new_size) } -> std::same_as<void>;
	}
	;

inline struct Null {
	auto bind() -> void {}
	auto unbind() -> void {}
	auto color_attachment_id() -> void* { return nullptr; }
	auto resize(const glm::vec2&) -> void {}
} null;

}// buffer::frame

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
	requires (T t, T other, const fs::path& path, const size_t slot) {
		{ t.width() } -> std::same_as<size_t>;
		{ t.height() } -> std::same_as<size_t>;
		{ t.bind(slot) } -> std::same_as<void>;
		{ t.unbind() } -> std::same_as<void>;
		{ t.native_handle() } -> std::convertible_to<void*>;
		{ t == other } -> std::same_as<bool>;
	}
	;

template <texture::Concept Texture>
struct Sub_Texture {
	using Coordinates = std::array<glm::vec2, 4>;

private:
	Texture& _parent;
	Coordinates coords;

public:
	struct Args {
		glm::vec2 cell_size;
		glm::uvec2 offset;
		glm::uvec2 sprite_size;	// in cells
	};
	Sub_Texture(Texture& _parent, Args&& a)
		: _parent{_parent}
		, coords{
		glm::vec2{ a.offset.x						* a.cell_size.x / _parent.width(),  a.offset.y						* a.cell_size.y / _parent.height() },
		glm::vec2{(a.offset.x + a.sprite_size.x)	* a.cell_size.x / _parent.width(),  a.offset.y						* a.cell_size.y / _parent.height() },
		glm::vec2{(a.offset.x + a.sprite_size.x)	* a.cell_size.x / _parent.width(), (a.offset.y + a.sprite_size.y)	* a.cell_size.y / _parent.height() },
		glm::vec2{ a.offset.x						* a.cell_size.x / _parent.width(), (a.offset.y + a.sprite_size.y)	* a.cell_size.y / _parent.height() }
		}
	{
		SAGE_ASSERT(
				rg::all_of(coords.begin(), coords.end(), [](const auto& c) { return glm::isNormalized(c, 1.f); }),
				"Given offset and size produce out of bounds Sub_Texture: {} {} {} {}",
				// No patience to deviate into specializing fmt...
				glm::to_string(coords[0]), glm::to_string(coords[1]), glm::to_string(coords[2]), glm::to_string(coords[3])
			);
	}

public:
	auto parent() const -> const Texture& { return _parent; }
	auto coordinates() const -> const Coordinates& { return coords; }
};

}// texture

namespace renderer {

namespace detail {

template <typename Renderer, typename Drawings, size_t... Idxs>
consteval auto _renderer_can_draw(const std::index_sequence<Idxs...>&) -> bool {
	return (requires (Renderer r, const typename Drawings::At<Idxs>& drawing, const typename Renderer::Draw_Args& args) {
				r.draw(drawing, args);
			} and ...
		);
}

template <typename Renderer, typename Drawings>
constexpr auto renderer_can_draw = _renderer_can_draw<Renderer, Drawings>(typename Drawings::Index_Sequence{});

}// renderer::detail

template <typename R>
concept Concept_2D =
	requires { typename R::Shader; } and shader::Concept<typename R::Shader>
	and requires { typename R::Vertex_Array; } and array::vertex::Concept<typename R::Vertex_Array>
	and requires { typename R::Draw_Args; } // and is type::Set<...>
	and requires { typename R::Drawings; } // and is type::Set<...>
		and (R::Drawings::size() > 0)
		and (R::Draw_Args::size() > 0)
		// TODO: Try to make this a constraint
		//and detail::renderer_can_draw<R, typename R::Drawings, typename R::Draw_Args>
	and requires { typename R::Frame_Buffer; } and buffer::frame::Concept<typename R::Frame_Buffer>
	and requires (R r, const camera::Orthographic& cam, const std::function<void()>& draws, const Event& e) {
		{ r.scene(cam, draws) } -> std::same_as<void>;
		{ r.event_callback(e) } -> std::same_as<void>;
		{ r.frame_buffer() } -> std::same_as<typename R::Frame_Buffer&>;
	}
	;

inline struct Null {
	using Shader = shader::Null;
	using Vertex_Array = array::vertex::Null;
	using Draw_Args = type::Set<std::any>;
	using Drawings = type::Set<std::any>;
	using Frame_Buffer = buffer::frame::Null;

	auto draw(const auto&, const auto&) -> void {}
	auto scene(const auto&, const auto&) -> void {}
	auto clear() -> void {}
	auto event_callback(const auto&) -> void {}
	auto frame_buffer() -> Frame_Buffer& {
		return buffer::frame::null;
	}
} null;
static_assert(Concept_2D<Null>);

template<texture::Concept Texture>
struct Batch {
	using Vertices = std::vector<buffer::vertex::Quad>;
	// TODO: Proper asset system and asset handles
	using Texture_Slots = std::vector<const Texture*>;

public:
	static constexpr auto max_quads = 10'000;
	static constexpr auto max_verteces = max_quads * 4;
	static constexpr auto max_indeces = max_quads * 6;
	// TODO: Query from GPU
	static constexpr auto max_texture_slots = 32u;

private:
	Vertices _verteces;
	Texture_Slots _texture_slots;
	size_t _indeces;

	const Texture default_texture = Texture{Size{1ul, 1ul}};

public:
	struct Capacity_Args { size_t verteces, texture_slots; };
	Batch(Capacity_Args&& caps)
	{
		_verteces.reserve(caps.verteces);
		_texture_slots.reserve(caps.texture_slots);

		_texture_slots.push_back(&default_texture);
	}

public:
	// TODO: More type safety
	auto push_texture(const Texture* tex) -> decltype(buffer::vertex::Quad::Texture::index) {
		SAGE_ASSERT(_texture_slots.size() < _texture_slots.capacity(), "Pushing more than {} textures", _texture_slots.capacity());

		const auto i = rg::find_if(
				_texture_slots | vw::drop(1),	// Scan after default texture
				[&] (const auto& x) { return *tex == *x; }
			);

		if (i == _texture_slots.end())
			_texture_slots.push_back(tex);

		return std::distance(_texture_slots.begin(), i);
	}

	// TODO: Make a namespace for shapes
	auto push_quad(std::array<buffer::vertex::Quad, 4>&& q) -> void {
		SAGE_ASSERT(_verteces.size() < max_verteces);

		rg::move(std::move(q), std::back_inserter(_verteces));
		_indeces += 6;
	}

public:
	auto clear_verteces() -> void {
		_verteces.clear();
		_indeces = 0;
	}

	auto clear_texture_slots() -> void {
		_texture_slots.erase(_texture_slots.begin() + 1, _texture_slots.end());	// Keep default at pos 0
	}

	auto clear() -> void {
		clear_verteces();
		clear_texture_slots();
	}

	auto verteces_are_empty() const -> bool {
		return _verteces.empty();
	}

	auto texture_slots_are_empty() const -> bool {
		return _texture_slots.size() == 1;	// Default at position 0
	}

	auto is_empty() const -> bool {
		return verteces_are_empty() and texture_slots_are_empty();
	}

public:
	auto indeces() const -> size_t {
		return _indeces;
	}

	auto verteces() const -> const Vertices& {
		return _verteces;
	}

	auto verteces_as_bytes() const -> std::span<const std::byte> {
		return std::as_bytes(std::span{_verteces});
	}

	auto texture_slots() const -> const Texture_Slots& {
		return _texture_slots;
	}
};

template <typename _Vertex_Array, typename _Texture, typename Draw_Call, typename Clear_Call, typename _Frame_Buffer, typename _Shader>
	requires
			array::vertex::Concept<_Vertex_Array>
		and texture::Concept<_Texture>
		and std::invocable<Draw_Call, const Batch<_Texture>&>
		and std::invocable<Clear_Call>
		and buffer::frame::Concept<_Frame_Buffer>
		and shader::Concept<_Shader>
struct Base_2D {
protected:
	using Vertex_Array = _Vertex_Array;
	using Texture = _Texture;
	using Sub_Texture = texture::Sub_Texture<Texture>;
	using Batch = renderer::Batch<Texture>;
	using Frame_Buffer = _Frame_Buffer;
	using Shader = _Shader;

protected:
	struct Scene_Data {
		Vertex_Array vertex_array;
		Frame_Buffer frame_buffer;
		Shader shader;
	};

protected:
	Scene_Data scene_data;
	Batch batch;

private:
	// TODO: Use scene_active only in debug mode
	bool scene_active = false;

	Draw_Call draw_call;

	Clear_Call clear_call;

	Profiler& profiler;

protected:
	Base_2D(Scene_Data&& sd, Profiler& prof = Profiler::global)
		: scene_data{std::move(sd)}
		, batch{{ .verteces = Batch::max_verteces, .texture_slots = Batch::max_texture_slots }}
		, profiler{prof}
	{}

public:
	// Should be called once from the App	//
	//
	// struct Magic {
	//   Renderer& renderer;
	//   auto render() { renderer.draw(...); }
	// };
	//
	// struct App {
	//		Renderer& renderer;
	//   	Camera& camera;
	//   	Magic magic;
	//
	//   	auto loop() {
	//	 	   renderer.scene(camera, [&] {
	//	 	   		magic.render();
	//	 	   	});
	//	 	}
	//	};
	//
	auto scene(const camera::Orthographic& cam, std::invocable auto&& draws) -> void {
		SAGE_ASSERT(not scene_active, "Must only call scene once: renderer.scene(camera, [] { render1(); render2(); });");

		scene_active = true;

		scene_data.frame_buffer.bind();

		std::invoke(clear_call);

		SAGE_ASSERT(batch.verteces_are_empty(), "Make sure to clear when flushing");

		scene_data.shader.bind();
		scene_data.shader.set("u_ViewProjection", cam.view_proj_mat());

		draws();

		flush();

		scene_data.frame_buffer.unbind();

		scene_active = false;
	}

	using Drawings = type::Set<Texture, Sub_Texture, glm::vec4>;

	struct Simple_Args {
		const glm::vec3& position;
		const glm::vec2& size;
		float rotation = 0.f;
	};

	using Draw_Args = type::Set<Simple_Args, glm::mat4>;

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
	template <typename Drawing, typename _Draw_Args>
		requires
				(Drawings::template contains<Drawing>())
			and (Draw_Args::template contains<_Draw_Args>())
	auto draw(const Drawing& drawing, const _Draw_Args& args) {
		using namespace sage::math;

		SAGE_ASSERT(scene_active);

		PROFILER_RENDERING(profiler, "Draw", [] (auto& result) { ++result.quads; });

		if (batch.indeces() >= Batch::max_indeces)
			flush();

		const auto transform = std::invoke([&] {
				if constexpr (std::same_as<_Draw_Args, Simple_Args>)
					return glm::translate(identity<glm::mat4>, args.position)
						* glm::rotate(identity<glm::mat4>, glm::radians(args.rotation), { 0.f, 0.f, 1.f })
						* glm::scale(identity<glm::mat4>, { args.size.x, args.size.y, 1.f })
						;
				else if constexpr (std::same_as<_Draw_Args, glm::mat4>)
					return args;
				else
					static_assert(false);
			});

		const auto verteces =
			transform
			* glm::mat4{
				-0.5f, -0.5f, 0.0f, 1.0f,
				 0.5f, -0.5f, 0.0f, 1.0f,
				 0.5f,  0.5f, 0.0f, 1.0f,
				-0.5f,  0.5f, 0.0f, 1.0f,
			}
			;

		const auto [color, tex_index, coords] = std::invoke([&] {
				constexpr auto full_drawing_coords = std::array{ glm::vec2{0.f,0.f}, glm::vec2{1.f,0.f}, glm::vec2{1.f,1.f}, glm::vec2{0.f,1.f} };
				constexpr auto default_color = glm::vec4{ 1.f, 1.f, 1.f, 1.f };

				if constexpr (std::same_as<Drawing, Texture>) {
					return std::make_tuple(default_color, batch.push_texture(&drawing), full_drawing_coords);
				}
				else if constexpr (std::same_as<Drawing, Sub_Texture>) {
					return std::make_tuple(default_color, batch.push_texture(&drawing.parent()), drawing.coordinates());
				}
				else if constexpr (std::same_as<Drawing, glm::vec4>)
					return std::make_tuple(drawing, 0.f, full_drawing_coords);
				else
					static_assert(false, "Unhandled type");
			});

		SAGE_ASSERT(coords.size() == static_cast<size_t>(verteces.length()) and verteces.length() == 4);

		auto verts = std::array<buffer::vertex::Quad, verteces.length()>{};
		for (const auto vertex : vw::iota(0ul, verts.size()))
			verts[vertex] = {
					.position = verteces[vertex],
					.color = color,
					.texture = {
						.coord = coords[vertex],
						.index = tex_index,
					},
				};
		batch.push_quad(std::move(verts));
	}

public:
	auto frame_buffer() -> Frame_Buffer& {
		return scene_data.frame_buffer;
	}

private:
	auto flush() -> void {
		SAGE_ASSERT(scene_active);

		PROFILER_RENDERING(profiler, "Flush", [] (auto& result) { ++result.draw_calls; });

		scene_data.vertex_array.bind();
		scene_data.vertex_array.vertex_buffer()
			.set_verteces(batch.verteces_as_bytes())
			;

		// Poor man's enumerate
		rg::for_each(batch.texture_slots(), [i = 0ul] (const auto& tex) mutable {
				tex->bind(i++);
			});

		std::invoke(draw_call, batch);

		batch.clear_verteces();
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
