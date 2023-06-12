#pragma once

#include "std.hpp"

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
#endif

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"	// Enable operator<<
#include "spdlog/sinks/stdout_color_sinks.h"

#define SAGE_LOG_TRACE(...)	SPDLOG_LOGGER_TRACE(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_DEBUG(...)	SPDLOG_LOGGER_DEBUG(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_INFO(...)	SPDLOG_LOGGER_INFO(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_WARN(...)	SPDLOG_LOGGER_WARN(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_ERROR(...)	SPDLOG_LOGGER_ERROR(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(::sage::Log::logger, __VA_ARGS__)

#define SAGE_ASSERT(cond) assert(cond)
#define SAGE_ASSERT_MSG(_cond_, _msg_)	\
	{	\
		const auto _cond_val_ = _cond_;	\
		if (not _cond_val_) { \
			SAGE_LOG_CRITICAL("{} -> {}: {}", #_cond_, _cond_val_, _msg_);	\
		}	\
		assert(((void)_msg_, _cond_val_));	\
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
