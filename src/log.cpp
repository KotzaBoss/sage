#include "log.hpp"

namespace sage::log {

// inline to only have one true static.
inline const Log::Logger Log::logger = spdlog::stderr_color_mt("SAGE");

inline const Log Log::log = Log();

Log::Log() {
	// [log_type] (time) ~thread logger file:line:function: message
	spdlog::set_pattern("%^[%-8l] (%T) ~%t %n %s:%#::%-20!!: %v%$");
	if constexpr (build::in_debug_mode)
		spdlog::set_level(spdlog::level::trace);
	else
		spdlog::set_level(spdlog::level::info);

	SAGE_LOG_INFO("Logging initialized");
}

}
