// Should always be at the end of sage.hpp

#pragma once

// Headers included in sage.hpp

auto main() -> int {
	prctl(PR_SET_NAME, "SAGE entry", 0, 0, 0);
	auto& app = sage::App::make();
	app.run();
}

