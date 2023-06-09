#include "sage.hpp"
#include "platform/linux/window.hpp"

namespace sage {
	auto App::make() -> App& {
		auto& win = Window::make(Window::Properties{});
		static auto app = App(win);
		return app;
	}
}
