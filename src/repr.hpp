#pragma once

#define REPR_DECL(type)	\
	friend auto operator<< (std::ostream& os, const type& obj) -> std::ostream&

#define REPR_DEF(type)	\
	auto operator<< (std::ostream& os, const type& obj) -> std::ostream&

#define REPR_DEF_FMT(type)	\
	REPR_DEF(type) { return os << fmt::format("{}", obj); }

#include "fmt/format.h"
#include "fmt/std.h"
#include "fmt/ranges.h"

#define FMT_FORMATTER(type)	\
	struct fmt::formatter<type>

#define FMT_FORMATTER_DEFAULT_PARSE	\
	constexpr auto parse(auto& ctx) -> auto {	\
		return ctx.end();	\
	}	\

#define FMT_FORMATTER_FORMAT(type)	\
	auto format(const type& obj, auto& ctx) -> auto

