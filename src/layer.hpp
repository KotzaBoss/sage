#pragma once

#include "std.hpp"

#include "event.hpp"
#include "ref_wrapper.hpp"
#include "util.hpp"

#include "repr.hpp"

namespace sage::layer {

#pragma message "TODO: Maybe pass optional<Event> to allow for functional chaining?"
template <typename Layer>
concept Concept =
	requires (Layer l, const Event& event, const std::chrono::milliseconds delta) {
		{ l.update(delta) } -> std::same_as<void>;
		// Must be called in layer::ImGui::new_frame()
		{ l.imgui_prepare() } -> std::same_as<void>;
		// layers.event_callback(window.pending_event());
		// instead of checking the pending_event first.
		{ l.event_callback(event) } -> std::same_as<void>;
	}
	;


template <layer::Concept... Ls>
struct Array : util::Polymorphic_Array<Ls...> {
	using Base = util::Polymorphic_Array<Ls...>;

public:
	Array(type::Any<Ls...> auto&&... ls)
		: Base{std::move(ls)...}
	{}

public:
	auto update(const std::chrono::milliseconds delta) -> void {
		Base::apply([=] (auto& layer) {
				layer.update(delta);
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

template <layer::Concept... Ls>
struct Storage : util::Polymorphic_Storage<Ls...> {
	using Base = util::Polymorphic_Storage<Ls...>;

	template<layer::Concept L>
	using Vector = typename Base::Vector<L>;

public:
	Storage(Vector<Ls>&&... layers)
		: Base{std::move(layers)...}
	{}

public:
	auto update(const std::chrono::milliseconds delta) -> void {
		Base::apply([=] (auto& layer) {
				layer.update(delta);
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

template <sage::layer::Concept... Ls>
FMT_FORMATTER(sage::layer::Array<Ls...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::layer::Array<Ls...>) {
		return fmt::format_to(ctx.out(), "layer::Array: {};", static_cast<const std::decay_t<decltype(obj)>::Base&>(obj));
	}
};

template <sage::layer::Concept... Ls>
FMT_FORMATTER(sage::layer::Storage<Ls...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::layer::Storage<Ls...>) {
		return fmt::format_to(ctx.out(), "layer::Storage: {};", static_cast<const std::decay_t<decltype(obj)>::Base&>(obj));
	}
};

