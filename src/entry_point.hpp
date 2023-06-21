// Should always be at the end of sage.hpp

#pragma once

// Headers included in sage.hpp

#ifndef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
auto main() -> int {
	prctl(PR_SET_NAME, "SAGE entry", 0, 0, 0);
	auto& app = sage::App::make();
	app.run();
}
#else
TEST_CASE ("App") {
	prctl(PR_SET_NAME, "SAGE entry", 0, 0, 0);
	auto& app = sage::App::make();
	app.run();
}
#endif

