#pragma once

#include "fmt/format.h"
#include "fmt/chrono.h"
#include "fmt/std.h"
#include "fmt/ranges.h"

#define FMT_FORMATTER(...)	\
	struct fmt::formatter<__VA_ARGS__>

#define FMT_FORMATTER_DEFAULT_PARSE	\
	constexpr auto parse(auto& ctx) -> auto {	\
		return ctx.end();	\
	}	\

#define FMT_FORMATTER_FORMAT(...)	\
	constexpr auto format(const __VA_ARGS__& obj, auto& ctx) const -> auto
