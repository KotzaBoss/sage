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

}
