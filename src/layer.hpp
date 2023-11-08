#pragma once

#include "std.hpp"

#include "graphics.hpp"
#include "event.hpp"
#include "input.hpp"
#include "util.hpp"

#include "repr.hpp"

namespace sage::layer {

#pragma message "TODO: Maybe pass optional<Event> to allow for functional chaining?"
#pragma message "TODO: Maybe pass renderer::Concept to .render()? Can there be multiple renderers?"
template <typename Layer, typename Input, typename Rendering>
concept Concept =
		input::Concept<Input>
	and graphics::renderer::Rendering<Rendering>
	and requires (Layer l, const Event& event, const std::chrono::milliseconds delta, typename Rendering::Renderer& renderer, Input& input) {
		{ l.update(delta, input) } -> std::same_as<void>;
		{ l.render(renderer) } -> std::same_as<void>;
		// Must be called in layer::ImGui::new_frame()
		{ l.imgui_prepare() } -> std::same_as<void>;
		// layers.event_callback(window.pending_event());
		// instead of checking the pending_event first.
		{ l.event_callback(event) } -> std::same_as<void>;
	}
	;

// Convinence to pack together template information:
//
// template <Rendering Ring, Spec... Ls>
// struct App {
// };
//
// struct Some_Rendering {
//		using Renderer = Linux_Renderer;
//		using Drawings = std::tuple<Linux_Texture, glm::vec4, int>;
// };
//
// struct Some_Layer_Spec {
//		using Layer = Some_Layer;
//		using Rendering = Some_Rendering;
// };
//
// using App<Some_Rendering,
//			Some_Layer_Spec
//     >;
//
template <typename L>
concept Spec =
		requires { typename L::Input; } and input::Concept<typename L::Input>
	and requires { typename L::Rendering; } and graphics::renderer::Rendering<typename L::Rendering>
	and requires { typename L::Layer; } and layer::Concept<typename L::Layer, typename L::Input, typename L::Rendering>
	;

template <layer::Spec... Ls>
	requires type::All<typename Ls::Input...>
struct Array : util::Polymorphic_Array<typename Ls::Layer...> {
	using Base = util::Polymorphic_Array<typename Ls::Layer...>;
	using Input = std::tuple_element_t<0, std::tuple<typename Ls::Input...>>;

public:
	Array(type::Any<typename Ls::Layer...> auto&&... ls)
		: Base{std::move(ls)...}
	{}

public:
	auto update(const std::chrono::milliseconds delta, Input& input) -> void {
		Base::apply([&] (auto& layer) {
				layer.update(delta, input);
			});
	}

	auto render(auto& renderer) -> void {
		Base::apply([&] (auto& layer) {
				layer.render(renderer);
			});
	}

	auto imgui_prepare() -> void {
		Base::apply([] (auto& layer) {
				layer.imgui_prepare();
			});
	}

	auto event_callback(const Event& e) -> void {
		Base::apply([&] (auto& layer) {
				layer.event_callback(e);
			});
	}

public:
	friend REPR_DEF_FMT(Array<Ls...>)
	friend FMT_FORMATTER(Array<Ls...>);

};

template <layer::Spec... Ls>
	requires type::All<typename Ls::Input...>
struct Storage : util::Polymorphic_Storage<typename Ls::Layer...> {
	using Base = util::Polymorphic_Storage<typename Ls::Layer...>;
	using Input = std::tuple_element_t<0, std::tuple<typename Ls::Input...>>;

public:
	Storage(typename Base::Vector<typename Ls::Layer>&&... layers)
		: Base{std::move(layers)...}
	{}

public:
	auto update(const std::chrono::milliseconds delta, Input& input) -> void {
		Base::apply([&] (auto& layer) {
				layer.update(delta, input);
			});
	}

	auto imgui_prepare() -> void {
		Base::apply([] (auto& layer) {
				layer.imgui_prepare();
			});
	}

	auto event_callback(const Event& e) -> void {
		Base::apply([&] (auto& layer) {
				layer.event_callback(e);
			});
	}

public:
	friend REPR_DEF_FMT(Storage<Ls...>)
	friend FMT_FORMATTER(Storage<Ls...>);
};

}// sage::layer

template <typename... Ls>
FMT_FORMATTER(sage::layer::Array<Ls...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::layer::Array<Ls...>) {
		return fmt::format_to(ctx.out(), "layer::Array: {};", static_cast<const std::decay_t<decltype(obj)>::Base&>(obj));
	}
};

template <typename... Ls>
FMT_FORMATTER(sage::layer::Storage<Ls...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::layer::Storage<Ls...>) {
		return fmt::format_to(ctx.out(), "layer::Storage: {};", static_cast<const std::decay_t<decltype(obj)>::Base&>(obj));
	}
};

