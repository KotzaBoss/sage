#pragma once

#include "std.hpp"

#include "core.hpp"
#include "math.hpp"
#include "log.hpp"
#include "repr.hpp"

namespace sage::inline event {

struct Event {
					// Type
	enum class Type {
		None = 0,
		Window_Closed, Window_Resized,
	};
	constexpr static inline auto bits_of_Type = bits<std::underlying_type_t<Type>>();
	REPR_DECL(Type);

					// Category
	// CAUTION: Do not forget to update the operator<< if the enum changes
	enum class Category : uint8_t {
		None = 0,
		Application	= 1 << 0,
		Input		= 1 << 1,
		Keyboard	= 1 << 2,
		Mouse		= 1 << 3
	};
	constexpr static inline auto bits_of_Category = bits<std::underlying_type_t<Category>>();
	REPR_DECL(Category);

					// Payloads
	using Payload = std::variant<
		std::monostate,	// No payload
		Size<size_t>
	>;
	static constexpr auto no_payload = Payload{};
	REPR_DECL(Payload);

	using Callback = std::function<void(const Event&)>;
	static constexpr auto uninitialized_callback = [] (const Event&) { SAGE_ASSERT_MSG(false, "Event::Callback is uninitialized"); };

public:
	Type type;
	Category category;
	Payload payload;

public:
	static auto make_window_closed	()							-> Event;
	static auto make_window_resized	(const Size<size_t>& sz)	-> Event;

public:
	REPR_DECL(Event);
};

struct Event_Dispatcher {
	// No clue
};

}// sage::event

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

