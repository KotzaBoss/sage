#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "sage.hpp"
#include "platform/linux/window.hpp"

namespace sage {
	auto App::make() -> App& {
		auto& win = Window::make(Window::Properties{});
		static auto app = App(win);
		MESSAGE(app);
		return app;
	}
}

