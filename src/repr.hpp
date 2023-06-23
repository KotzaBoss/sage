#pragma once

#define REPR_DECL(type)	\
	friend auto operator<< (std::ostream& os, const type& obj) -> std::ostream&

#define REPR_DEF(type)	\
	auto operator<< (std::ostream& os, const type& obj) -> std::ostream&

#define REPR_DEF_FMT(type)	\
	REPR_DEF(type) { return os << fmt::format("{}", obj); }

