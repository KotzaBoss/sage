#pragma once

#include "std.hpp"

#include "event.hpp"
#include "math.hpp"
#include "repr.hpp"

namespace sage::window {

struct Properties {
	using Size = sage::Size<size_t>;

	std::string title = "SAGE Window"s;
	Size size = { .width=1280, .height=720 };

	REPR_DECL(Properties);
};

template <typename Window>
concept Concept =
	requires (Window win, Properties&& properties, Event::Callback&& event_callback, const Event& event) {
		Window(std::move(properties));	// Constructor
		{ win.setup(std::move(event_callback)) } -> std::same_as<void>;
		{ win.update() } -> std::same_as<void>;
		{ win.teardown() } -> std::same_as<void>;
		{ win.event_callback(event) } -> std::same_as<void>;
		{ win.properties() } -> std::same_as<const Properties&>;
	}
	;

}// sage::window

template <>
FMT_FORMATTER(sage::window::Properties) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::window::Properties) {
		return fmt::format_to(ctx.out(), "Properties title={:?} size={};", obj.title, obj.size);
	}
};

