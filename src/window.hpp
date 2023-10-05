#pragma once

#include "std.hpp"

#include "event.hpp"
#include "math.hpp"
#include "repr.hpp"
#include "util.hpp"

namespace sage::window {

struct Properties {
	using Size = sage::Size<size_t>;

	std::string_view title = "SAGE Window"sv;
	Size size = { .width=1280, .height=720 };
	bool is_minimized = false;

	REPR_DECL(Properties);
};

template <typename Window>
concept Concept =
	requires (Window win, Properties&& properties, const Event& event) {
		Window(std::move(properties));	// Constructor
		{ win.update() } -> std::same_as<void>;
		{ win.consume_pending_event() } -> std::same_as<std::optional<Event>>;
		{ win.properties() } -> std::same_as<Properties>;
		{ win.native_handle() } -> std::convertible_to<void*>;	// Each concrete provides its own pointer type
	}
	;

struct Base {
protected:
	// Ensure data is trivially copyable
	std::atomic<window::Properties> _properties;
	std::atomic<std::optional<Event>> _pending_event;

public:
	Base(Base&& other) {
		_properties = other._properties.load();
		_pending_event = other._pending_event.load();
	};

	Base(Properties&& ps)
		: _properties{std::move(ps)}
	{}

	auto properties() const -> window::Properties {
		return _properties.load();
	}

	auto consume_pending_event() -> std::optional<Event> {
		return _pending_event.exchange(std::nullopt);
	}

	auto is_minimized() const -> bool {
		return _properties.load().is_minimized;
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

