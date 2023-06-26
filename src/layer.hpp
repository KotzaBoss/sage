#pragma once

#include "std.hpp"

#include "event.hpp"
#include "ref_wrapper.hpp"
#include "util.hpp"

#include "repr.hpp"

namespace sage::layer {

template <typename Layer>
concept Concept =
	requires (Layer l, const Event& event) {
		{ l.setup() } -> std::same_as<void>;
		{ l.update() } -> std::same_as<void>;
		{ l.teardown() } -> std::same_as<void>;
		{ l.event_callback(event) } -> std::same_as<void>;
	}
	;


template <layer::Concept... Ls>
struct Storage : util::Polymorphic_Storage<Ls...> {

public:
	Storage(auto&&... layers)
		: util::Polymorphic_Storage<Ls...>{std::move(layers)...}
	{}

public:
	auto setup()	-> void { this->apply([] (auto& layer) { layer.setup();		}); }
	auto update()	-> void { this->apply([] (auto& layer) { layer.update();	}); }
	auto teardown()	-> void { this->apply([] (auto& layer) { layer.teardown();	}); }

	auto event_callback(const Event& e) -> void { this->apply([&] (auto& layer) { layer.event_callback(e); });}

public:
	friend REPR_DEF_FMT(Storage<Ls...>)
	friend FMT_FORMATTER(Storage<Ls...>);
};

}// sage::layer

template <sage::layer::Concept... Ls>
FMT_FORMATTER(sage::layer::Storage<Ls...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::layer::Storage<Ls...>) {
		fmt::format_to(ctx.out(), "layer::Storage: ");
		obj.const_apply([&] (const auto& layer) { fmt::format_to(ctx.out(), "\n\t{}", layer); });
		return fmt::format_to(ctx.out(), "\n\t;");
	}
};

