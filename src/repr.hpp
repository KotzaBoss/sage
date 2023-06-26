#pragma once

#define REPR_DECL(...)	\
	friend auto operator<< (std::ostream& os, const __VA_ARGS__& obj) -> std::ostream&

#define REPR_DEF(...)	\
	auto operator<< (std::ostream& os, const __VA_ARGS__& obj) -> std::ostream&

#define REPR_DEF_FMT(...)	\
	REPR_DEF(__VA_ARGS__) { return os << fmt::format("{}", obj); }

#include "fmt/format.h"
#include "fmt/std.h"
#include "fmt/ranges.h"

#define FMT_FORMATTER(...)	\
	struct fmt::formatter<__VA_ARGS__>

#define FMT_FORMATTER_DEFAULT_PARSE	\
	constexpr auto parse(auto& ctx) -> auto {	\
		return ctx.end();	\
	}	\

#define FMT_FORMATTER_FORMAT(...)	\
	auto format(const __VA_ARGS__& obj, auto& ctx) const -> auto

