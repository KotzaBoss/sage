#pragma once

#include "std.hpp"

#include "fmt/format.h"
#include "fmt/chrono.h"
#include "fmt/std.h"
#include "fmt/ranges.h"

#ifdef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// Passing the format arguments to spdlog through __VA_ARGS__ is abit clunky so
// to make sure it works consistently try passing the format+args verbosely, meaning
// always pass both.
//
// SAGE_ASSERT(may_be_bad(), "{}", 123);	// ok
// SAGE_ASSERT(may_be_bad(), 123);			// not ok
//
// The latter example could work for the SAGE_LOG_* macros.

#define SAGE_LOG_TRACE(...)	SPDLOG_LOGGER_TRACE(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_DEBUG(...)	SPDLOG_LOGGER_DEBUG(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_INFO(...)	SPDLOG_LOGGER_INFO(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_WARN(...)	SPDLOG_LOGGER_WARN(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_ERROR(...)	SPDLOG_LOGGER_ERROR(::sage::Log::logger, __VA_ARGS__)
#define SAGE_LOG_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(::sage::Log::logger, __VA_ARGS__)

#define SAGE_ASSERT(expr, ...)	\
	{\
		if (not sage::truth(expr)) {	\
			SAGE_LOG_CRITICAL("`{}` evaluated to false.", #expr);	\
			if (not sage::log::detail::variadic_pack_is_empty(__VA_ARGS__))	\
				/* spdlog doesn't use __VA_OPT__ so add an "" to make it work when __VA_ARGS__ is empty */	\
				SAGE_LOG_CRITICAL("\t" __VA_ARGS__);	\
			\
			std::terminate();	\
		}	\
	}\
	(void)0

#define SAGE_ASSERT_PATH_EXISTS(path)	SAGE_ASSERT(fs::exists(path), "{}", fs::current_path()/path);

// Check potential errors when reading the file_size of path
#define SAGE_ASSERT_PATH_READABLE(path)	\
	{	\
		SAGE_ASSERT_PATH_EXISTS(path);	\
		auto err = std::error_code{};	\
		[[maybe_unused]] const auto _ = fs::file_size(path, err);	\
		SAGE_ASSERT(not err, "{}: {}", path, err.message());	\
	}	\
	(void)0

namespace sage::inline log {

namespace detail {
constexpr auto variadic_pack_is_empty(auto&&... xs) -> size_t {
	return sizeof...(xs) == 0;
}
}// detail

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
