#pragma once

#include "window.hpp"

namespace sage::inline app {

struct App {

private:
	Window& window;

public:
	App(Window& win);

public:
	// Defined by client
	static auto make() -> App&;

public:
	auto run() -> void;
	auto event_callback(const Event& e) -> void;
};

}// sage
