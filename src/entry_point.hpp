// Should always be at the end of sage.hpp

#pragma once

// Headers included in sage.hpp

auto main() -> int {
	prctl(PR_SET_NAME, "SAGE entry", 0, 0, 0);
	sage::Log::init();
	SAGE_LOG_TRACE("t");
	SAGE_LOG_DEBUG("d");
	float i = 123.456f;
	SAGE_LOG_INFO("i {}", i);
	SAGE_LOG_WARN("w");
	SAGE_LOG_ERROR("e");
	SAGE_LOG_CRITICAL("c");
	auto app = sage::App::make();
	app.run();
}

