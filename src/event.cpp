#include "event.hpp"

namespace sage::event {

auto Event::make_window_closed() -> Event {
	return {
		.type = Type::Window_Closed,
		.category = Category::Input,
		.payload = Event::no_payload
	};
}

auto Event::make_window_resized(const Size<size_t>& sz) -> Event {
	return {
		.type = Type::Window_Resized,
		.category = Category::Input,
		.payload = sz
	};
}

auto Event::make_mouse_button(const Make_Mouse_Button_Args& args) -> Event {
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

auto Event::make_mouse_scroll(const input::Mouse::Scroll& scroll) -> Event {
	return {
		.type = Event::Type::Mouse_Scrolled,
		.category = Category::Mouse,
		.payload = scroll
	};
}

auto Event::make_key(const Make_Key_Args& args) -> Event {
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

}// sage::event
