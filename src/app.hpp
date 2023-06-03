#pragma once

namespace sage::inline app {

struct App {
	auto run() -> void;

	// Defined by client
	static auto make() -> App;
};

}// sage
