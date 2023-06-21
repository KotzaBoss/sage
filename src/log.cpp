#include "log.hpp"

namespace sage::log {

// inline to only have one true static.
inline const Log::Logger Log::logger = spdlog::stderr_color_mt("SAGE");

inline const Log Log::log = Log();

Log::Log() {
	// log_type (time) ~thread logger: message
	spdlog::set_pattern("%^%-8l (%T) ~%t %n: %v%$");
	SAGE_LOG_INFO("Logging initialized");
}

}