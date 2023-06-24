#pragma once

#include "std.hpp"
#include "log.hpp"

#include "repr.hpp"

namespace sage::inline math {

template<typename T>
concept Number = std::integral<T> or std::floating_point<T>;

template<Number N = size_t>
struct Size {
	using Type = N;

public:
	N width, height;

public:
	template <Number New_N>
	static auto to(const Size<N>& s) -> Size<New_N> {
		SAGE_ASSERT(std::in_range<New_N>(s.width) and std::in_range<New_N>(s.height));
		return { .width = static_cast<New_N>(s.width), .height = static_cast<New_N>(s.height) };
	}

public:
	friend
	REPR_DEF_FMT(Size<N>)
};

}// sage::math

template <sage::Number N>
FMT_FORMATTER(sage::Size<N>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::Size<N>) {
		return fmt::format_to(ctx.out(), "Size: width={:4} height={:4};", obj.width, obj.height);
	}
};
