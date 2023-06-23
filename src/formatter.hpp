#pragma once

#include "fmt/core.h"
#include "fmt/std.h"

#include "log.hpp"

#define FMT_FORMATTER(type)	\
	struct fmt::formatter<type>

#define FMT_FORMATTER_DEFAULT_PARSE	\
	constexpr auto parse(auto& ctx) -> auto {	\
		return ctx.end();	\
	}	\

#define FMT_FORMATTER_FORMAT(type)	\
	auto format(const type& obj, auto& ctx) -> auto


#include "math.hpp"

template <sage::Number N>
FMT_FORMATTER(sage::Size<N>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::Size<N>) {
		return fmt::format_to(ctx.out(), "Size: width={:4} height={:4};", obj.width, obj.height);
	}
};

#include "event.hpp"

template <>
FMT_FORMATTER(sage::Event::Type) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::Event::Type) {
		return fmt::format_to(ctx.out(),
				"Type: {};",
				std::invoke([&] {
					switch (obj) {
						case sage::Event::Type::Window_Closed:		return "Window_Closed";
						case sage::Event::Type::Window_Resized:		return "Window_Resized";
						case sage::Event::Type::None:				return "None";
						default:
							return "BAD";
					}
				})
			);
	}
};

template <>
FMT_FORMATTER(sage::Event::Category) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::Event::Category) {
		return fmt::format_to(ctx.out(),
				"Category: {};",
				std::invoke([&] {
					switch (obj) {
						case sage::Event::Category::Application:	return "Application";
						case sage::Event::Category::Input:			return "Input";
						case sage::Event::Category::Keyboard:		return "Keyboard";
						case sage::Event::Category::Mouse:			return "Mouse";
						default:
							return "BAD";
					}
				})
			);
	}
};

template <>
FMT_FORMATTER(sage::Event::Payload) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::Event::Payload) {
		return std::visit(
			[&] (auto&& x) {
				return fmt::format_to(ctx.out(), "Payload: {};", x);
			},
			obj
		);
	}
};

template <>
FMT_FORMATTER(sage::Event) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::Event) {
		return fmt::format_to(ctx.out(), "Event:\ttype={}\tcategory={}\tpayload={};", obj.type, obj.category, obj.payload);
	}
};

#include "window.hpp"

template <>
FMT_FORMATTER(sage::Window::Properties) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::Window::Properties) {
		return fmt::format_to(ctx.out(), "Properties title={:?} size={};", obj.title, obj.size);
	}
};

template <>
FMT_FORMATTER(sage::Window) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::Window) {
		return fmt::format_to(ctx.out(), "Window: properties={};", obj.properties());
	}
};

#include "platform/linux/window.hpp"

template <>
FMT_FORMATTER(sage::oslinux::Window) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::oslinux::Window) {
		return fmt::format_to(ctx.out(), "oslinux::{}", static_cast<const sage::Window&>(obj));
	}
};

#include "app.hpp"

template <>
FMT_FORMATTER(sage::App) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::App) {
		return fmt::format_to(ctx.out(), "App: window={};", obj.window());
	}
};

