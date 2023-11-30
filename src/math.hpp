#pragma once

#include "src/std.hpp"
#include "src/util.hpp"
#include "src/log.hpp"

#include "src/repr.hpp"

#include "src/glm.hpp"

namespace sage::inline math {

template<typename T>
concept Number = std::integral<T> or std::floating_point<T>;

template <Number Range, Number X>
constexpr auto in_range(X x) -> bool {
	if constexpr (std::integral<Range>)
		return std::in_range<Range>(x);
	else {
		using limits = std::numeric_limits<Range>;
		return limits::min() <= x and x <= limits::max();
	}
}

// FIXME: default template does not work.
template<Number N = size_t>
struct Size {
	using Type = N;

public:
	N width, height;

public:
	template <Number New_N>
	auto to() const -> Size<New_N> {
		SAGE_ASSERT(in_range<New_N>(width) and in_range<New_N>(height));
		return { .width = static_cast<New_N>(width), .height = static_cast<New_N>(height) };
	}

	auto is_zero() const -> bool {
		if constexpr (std::floating_point<N>)
			SAGE_LOG_WARN("Caution testing Size<floating_point> for equality with 0");

		return width == 0 and height == 0;
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
