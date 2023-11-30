#pragma once

#include "src/std.hpp"

#include "src/log.hpp"
#include "src/repr.hpp"

namespace sage::input {

enum class Key {
	None = -1,
	Left_Ctrl,
	Space,
	// Letters
	Q, E, W, A, S, D,
};

struct Mouse {
	enum Button {
		None = -1, Left = 0, Right = 1, Middle = 2,
	};

	struct Pos {
		double x, y;

		REPR_DECL(Pos);
	};

	struct Scroll {
		struct {
			double x, y;
		} offset;

		REPR_DECL(Scroll);
	};
};

// TODO: OpenGL does not support mouse scroll polling, make the input Concept have a mouse_scroll() -> optional<Mouse::Scroll> ?
template <typename Input>
concept Concept =
	requires (Input i, const Key& k, const Mouse::Button& b) {
		{ i.is_key_pressed(k) } -> std::same_as<bool>;
		{ i.is_mouse_button_pressed(b) } -> std::same_as<bool>;
		{ i.mouse_pos() } -> std::same_as<Mouse::Pos>;
		//{ i.mouse_scroll() } -> std::same_as<Mouse::Scroll>;
	}
	;

struct Null {
	auto is_key_pressed(const Key&) -> bool {
		return false;
	}

	auto is_mouse_button_pressed(const Mouse::Button&) -> bool {
		return false;
	}

	auto mouse_pos() -> Mouse::Pos {
		return {0, 0};
	}
};

inline Null null;

} //sage::input

template <>
FMT_FORMATTER(sage::input::Key) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::input::Key) {
		return fmt::format_to(ctx.out(),
				"input::Key: {};",
				std::invoke([&] {
					switch (obj) {
						case sage::input::Key::Q:		return "Q";
						case sage::input::Key::E:		return "E";
						case sage::input::Key::W:		return "W";
						case sage::input::Key::A:		return "A";
						case sage::input::Key::S:		return "S";
						case sage::input::Key::D:		return "D";
						default: return "BAD input::Key";
					}
				})
			);
	}
};

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

template <>
FMT_FORMATTER(sage::input::Mouse::Scroll) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::input::Mouse::Scroll) {
		return fmt::format_to(ctx.out(), "input::Mouse::Scroll: offset.x={} offset.y={};", obj.offset.x, obj.offset.y);
	}
};
