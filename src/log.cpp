#include "log.hpp"

namespace sage::log {

const Log::Logger Log::logger = spdlog::stderr_color_mt("SAGE");

const Log Log::log = Log();

Log::Log() {
	// log_type (time) ~thread logger: message
	spdlog::set_pattern("%^%-8l (%T) ~%t %n: %v%$");
}

}
