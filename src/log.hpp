#pragma once

#include "std.hpp"

#include "fmt/format.h"
#include "fmt/chrono.h"
#include "fmt/std.h"

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

#define SAGE_ASSERT_MSG(cond, ...)	\
	{	\
		const auto cond_val = sage::truth(cond);	\
		if (not cond_val) { \
			SAGE_LOG_CRITICAL("{} : {}", #cond, cond_val);	\
			SAGE_LOG_CRITICAL(__VA_ARGS__);	\
		}	\
		assert(cond_val);	\
	}	\
	(void)0

#define SAGE_ASSERT_PATH_EXISTS(path)	SAGE_ASSERT_MSG(fs::exists(path), "{}", fs::current_path()/path);

// Check potential errors when reading the file_size of path
#define SAGE_ASSERT_PATH_READABLE(path)	\
	{	\
		SAGE_ASSERT_PATH_EXISTS(path);	\
		auto err = std::error_code{};	\
		[[maybe_unused]] const auto _ = fs::file_size(path, err);	\
		SAGE_ASSERT_MSG(not err, "{}: {}", path, err.message());	\
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
