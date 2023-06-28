#pragma once

#include "std.hpp"

#include "repr.hpp"

namespace sage::input {

enum class Key {
	None = -1, Left_Ctrl,
};

struct Mouse {
	enum Button {
		None = -1, Left = 0, Right = 1, Middle = 2,
	};

	struct Pos {
		double x, y;

		REPR_DECL(Pos);
	};
};


template <typename Input>
concept Concept =
	requires (Input i, const Key& k, const Mouse::Button& b) {
		{ i.is_key_pressed(k) } -> std::same_as<bool>;
		{ i.is_mouse_button_pressed(b) } -> std::same_as<bool>;
		{ i.mouse_pos() } -> std::same_as<Mouse::Pos>;
	}
	;

} //sage::input


template <>
FMT_FORMATTER(sage::input::Mouse::Button) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::input::Mouse::Button) {
		return fmt::format_to(ctx.out(),
				"input::Mouse::Button: {};",
				std::invoke([&] {
					switch (obj) {
						case sage::input::Mouse::Button::Left:	return "Left";
						case sage::input::Mouse::Button::Right:	return "Right";
						case sage::input::Mouse::Button::Middle:	return "Middle";
						default:
							return "BAD";
					}
				})
			);
	}
};

template <>
FMT_FORMATTER(sage::input::Mouse::Pos) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::input::Mouse::Pos) {
		return fmt::format_to(ctx.out(), "input::Mouse::Pos: x={} y={};", obj.x, obj.y);
	}
};
