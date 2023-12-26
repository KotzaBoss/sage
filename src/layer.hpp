#pragma once

#include "src/std.hpp"

#include "src/graphics.hpp"
#include "src/event.hpp"
#include "src/input.hpp"
#include "src/util.hpp"

#include "src/repr.hpp"

namespace sage::layer {

// TODO: Maybe pass optional<Event> to allow for functional chaining?
// TODO: Maybe pass renderer::Concept to .render()? Can there be multiple renderers?
template <typename Layer, typename Input, typename Rendering, typename User_State>
concept Concept =
		input::Concept<Input>
	and graphics::renderer::Rendering<Rendering>
	and requires (Layer l, const Event& event, const std::chrono::milliseconds delta, typename Rendering::Renderer& renderer, Input& input, User_State& user_state) {
		{ l.update(delta, input, user_state) } -> std::same_as<void>;
		{ l.render(renderer, user_state) } -> std::same_as<void>;
		// Must be called in layer::ImGui::new_frame()
		{ l.imgui_prepare(user_state) } -> std::same_as<void>;
		// layers.event_callback(window.pending_event());
		// instead of checking the pending_event first.
		{ l.event_callback(event, user_state) } -> std::same_as<void>;
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
	and requires { typename L::User_State; }
	and requires { typename L::Layer; }
		and layer::Concept<typename L::Layer, typename L::Input, typename L::Rendering, typename L::User_State>
	;

struct Null_User_State {};
inline auto null_user_state = Null_User_State{};

template <layer::Spec... Ls>
	requires	// All specs to share the same Input, Rendering, User_State, ...
			type::All<typename Ls::Input...>
		and type::All<typename Ls::Rendering...>
		and type::All<typename Ls::User_State...>
struct Array : util::Polymorphic_Array<typename Ls::Layer...> {
	using Base = util::Polymorphic_Array<typename Ls::Layer...>;
	using Input = type::Front<typename Ls::Input...>;
	using Renderer = type::Front<typename Ls::Rendering::Renderer...>;
	using User_State = type::Front<typename Ls::User_State...>;

public:
	Array(type::Any<typename Ls::Layer...> auto&&... ls)
		: Base{std::move(ls)...}
	{}

public:
	auto update(const std::chrono::milliseconds delta, Input& input, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.update(delta, input, user_state);
			});
	}

	auto render(Renderer& renderer, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.render(renderer, user_state);
			});
	}

	auto imgui_prepare(User_State& us) -> void {
		Base::apply([&] (auto& layer) {
				layer.imgui_prepare(us);
			});
	}

	auto event_callback(const Event& e, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.event_callback(e, user_state);
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
	using Input = type::Front<typename Ls::Input...>;
	using Renderer = type::Front<typename Ls::Rendering::Renderer...>;
	using User_State = type::Front<typename Ls::User_State...>;

public:
	Storage(typename Base::Vector<typename Ls::Layer>&&... layers)
		: Base{std::move(layers)...}
	{}

public:
	auto update(const std::chrono::milliseconds delta, Input& input, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.update(delta, input, user_state);
			});
	}

	auto imgui_prepare(User_State& us) -> void {
		Base::apply([&] (auto& layer) {
				layer.imgui_prepare(us);
			});
	}

	auto render(Renderer& renderer, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.render(renderer, user_state);
			});
	}

	auto event_callback(const Event& e, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.event_callback(e, user_state);
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

