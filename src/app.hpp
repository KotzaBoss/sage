#pragma once

#include <thread>

#include "window.hpp"
#include "repr.hpp"

namespace sage::inline app {

struct App {

private:
	Window& _window;
	std::jthread loop;

public:
	App(Window& win);
	~App();

public:
	// Defined by client
	static auto make() -> App&;

public:
	auto start() -> void;
	auto stop() -> void;

	auto event_callback(const Event& e) -> void;

public:
	inline auto window() const -> const Window& { return _window; }

public:
	REPR_DECL(App);
};

}// sage
