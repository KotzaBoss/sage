#pragma once

#define REPR_DECL(...)	\
	friend auto operator<< (std::ostream& os, const __VA_ARGS__& obj) -> std::ostream&

#define REPR_DEF(...)	\
	auto operator<< (std::ostream& os, const __VA_ARGS__& obj) -> std::ostream&

#define REPR_DEF_FMT(...)	\
	REPR_DEF(__VA_ARGS__) { return os << fmt::format("{}", obj); }

#include "src/fmt.hpp"
