#include "log.hpp"

namespace sage::log {

std::shared_ptr<spdlog::logger> Log::logger;

auto Log::init() -> void {
	spdlog::set_pattern("%^%-8l (%T) ~%t %n: %v%$");	// log type (time) ~thread logger: message
	logger = spdlog::stderr_color_mt("SAGE");
}

}
