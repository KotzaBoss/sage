#pragma once

#include "std.hpp"

#include "event.hpp"
#include "math.hpp"
#include "repr.hpp"
#include "util.hpp"

namespace sage::window {

struct Properties {
	using Size = sage::Size<size_t>;

	std::string title = "SAGE Window"s;
	Size size = { .width=1280, .height=720 };

	REPR_DECL(Properties);
};

template <typename Window>
concept Concept =
	requires (Window win, Properties&& properties, const Event& event) {
		Window(std::move(properties));	// Constructor
		{ win.setup() } -> std::same_as<void>;
		{ win.update() } -> std::same_as<void>;
		{ win.teardown() } -> std::same_as<void>;
		{ win.pending_event() } -> std::same_as<std::optional<Event>>;
		{ win.properties() } -> std::same_as<const Properties&>;
		{ win.native_handle() } -> std::convertible_to<void*>;	// Each concrete provides its own pointer type
	}
	;

struct Base {
protected:
	sage::window::Properties _properties;
	Monitor<std::optional<Event>> _pending_event;

public:
	Base(Base&&) = default;

	Base(Properties&& ps)
		: _properties{std::move(ps)}
	{}

	auto properties() const -> const sage::window::Properties& { return _properties; }

	auto pending_event() const -> std::optional<Event> {
		return _pending_event.release();
	}
};

}// sage::window

template <>
FMT_FORMATTER(sage::window::Properties) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::window::Properties) {
		return fmt::format_to(ctx.out(), "Properties title={:?} size={};", obj.title, obj.size);
	}
};

