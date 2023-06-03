// Should always be at the end of sage.hpp

#pragma once

// Headers included in sage.hpp

auto main() -> int {
	std::cerr << "Sage\n";
	auto app = sage::App::make();
	app.run();
}

