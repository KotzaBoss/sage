#pragma once

#include "src/std.hpp"

#include "src/util.hpp"
#include "src/input.hpp"
#include "src/math.hpp"
#include "src/log.hpp"
#include "src/repr.hpp"

namespace sage::inline event {

struct Event {
					// Type
	enum class Type {
		None = 0,
		Window_Closed, Window_Resized, Window_Minimized, Window_Restored,
		Mouse_Button_Pressed, Mouse_Button_Released, Mouse_Button_Repeated, Mouse_Moved, Mouse_Scrolled,
		Key_Pressed, Key_Repeated, Key_Released,
	};
	constexpr static inline auto bits_of_Type = bits<std::underlying_type_t<Type>>;
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
	constexpr static inline auto bits_of_Category = bits<std::underlying_type_t<Category>>;
	REPR_DECL(Category);

					// Payloads
	using Payload = std::variant<
		std::monostate,	// No payload
		Size<size_t>,
		bool,
		input::Mouse::Button,
		input::Mouse::Scroll,
		input::Key
	>;
	static constexpr auto no_payload = Payload{};
	REPR_DECL(Payload);

public:
	Type type;
	Category category;
	Payload payload;

public:
	static auto make_window_closed() -> Event {
		return {
			.type = Type::Window_Closed,
			.category = Category::Input,
			.payload = Event::no_payload
		};
	}

	static auto make_window_resized(const Size<size_t>& sz)	-> Event {
		return {
			.type = Type::Window_Resized,
			.category = Category::Input,
			.payload = sz
		};
	}

	static auto make_window_minimized() -> Event {
		return {
			.type = Type::Window_Minimized,
			.category = Category::Input,
			.payload = Event::no_payload
		};
	}

	static auto make_window_restored() -> Event {
		return {
			.type = Type::Window_Restored,
			.category = Category::Input,
			.payload = Event::no_payload
		};
	}

	struct Make_Mouse_Button_Args {
		Type type;
		input::Mouse::Button mouse_button;
	};
	static auto make_mouse_button(const Make_Mouse_Button_Args& args)	-> Event {
		SAGE_ASSERT(
				args.type == Event::Type::Mouse_Button_Pressed
				or args.type == Event::Type::Mouse_Button_Released
			);

		return {
			.type = args.type,
			.category = Category::Mouse,
			.payload = args.mouse_button
		};
	}

	static auto make_mouse_scroll(const input::Mouse::Scroll& args) -> Event {
		return {
			.type = Event::Type::Mouse_Scrolled,
			.category = Category::Mouse,
			.payload = args
		};
	}

	struct Make_Key_Args {
		Type type;
		input::Key key;
	};
	static auto make_key(const Make_Key_Args& args) -> Event {
		SAGE_ASSERT(
				args.type == Event::Type::Key_Pressed
				or args.type == Event::Type::Key_Repeated
				or args.type == Event::Type::Key_Released
			);

		return {
			.type = args.type,
			.category = Category::Keyboard,
			.payload = args.key
		};
	}

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
						case sage::Event::Type::Window_Closed:			return "Window_Closed";
						case sage::Event::Type::Window_Resized:			return "Window_Resized";
						case sage::Event::Type::Window_Minimized:		return "Window_Minimized";
						case sage::Event::Type::Window_Restored:		return "Window_Restored";
						case sage::Event::Type::Mouse_Button_Pressed:	return "Mouse_Button_Pressed";
						case sage::Event::Type::Mouse_Button_Released:	return "Mouse_Button_Released";
						case sage::Event::Type::Mouse_Scrolled:			return "Mouse_Scrolled";
						case sage::Event::Type::Key_Pressed:			return "Key_Pressed";
						case sage::Event::Type::Key_Repeated:			return "Key_Repeated";
						case sage::Event::Type::Key_Released:			return "Key_Released";
						case sage::Event::Type::None:					return "None";
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
						case sage::Event::Category::None:			return "None";
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

namespace sage::event {
REPR_DEF_FMT(Event::Type);
REPR_DEF_FMT(Event::Category);
REPR_DEF_FMT(Event::Payload);
REPR_DEF_FMT(Event);
}

#ifdef SAGE_TEST_EVENT
TEST_CASE ("Event") {
	using namespace sage;
	MESSAGE(Event::make_window_closed());
	for (auto i = 0; i < 10; ++i)
		MESSAGE(Event::make_window_resized(Size<size_t>{(size_t)std::rand() % 4000, (size_t)std::rand() % 4000}));
}
#endif

