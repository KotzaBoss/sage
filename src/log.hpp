#pragma once

#include "std.hpp"

#include "fmt/format.h"

#ifdef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define SAGE_LOG_TRACE(...)	SPDLOG_LOGGER_TRACE(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_DEBUG(...)	SPDLOG_LOGGER_DEBUG(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_INFO(...)	SPDLOG_LOGGER_INFO(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_WARN(...)	SPDLOG_LOGGER_WARN(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_ERROR(...)	SPDLOG_LOGGER_ERROR(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(::sage::Log::logger, __VA_ARGS__)

#define SAGE_ASSERT(cond) assert(cond)

#define SAGE_ASSERT_MSG(cond, msg)	\
	{	\
		const auto cond_val = cond;	\
		if (not cond_val) { \
			SAGE_LOG_CRITICAL("{} == {}: {}", #cond, cond_val, msg);	\
		}	\
		assert(((void)msg, cond_val));	\
	}	\
	(void)0

namespace sage::inline log {

struct Log {
	using enum spdlog::level::level_enum;
	using Logger = std::shared_ptr<spdlog::logger>;

public:
	static const Logger logger;

private:
	static const Log log;

public:
	Log();
};

}
