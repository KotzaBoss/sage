#pragma once

#include "window.hpp"
#include "repr.hpp"

namespace sage::inline app {

struct App {

private:
	Window& _window;

public:
	App(Window& win);

public:
	// Defined by client
	static auto make() -> App&;

public:
	auto run() -> void;
	auto event_callback(const Event& e) -> void;

public:
	inline auto window() const -> const Window& { return _window; }

public:
	REPR_DECL(App);
};

}// sage
