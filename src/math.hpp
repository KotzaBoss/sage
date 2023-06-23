#pragma once

#include "std.hpp"
#include "log.hpp"

namespace sage::inline math {

template<typename T>
concept Number = std::integral<T> or std::floating_point<T>;

template<Number N = size_t>
struct Size {
	using Type = N;

	N width, height;


	template <Number New_N>
	static auto to(const Size<N>& s) -> Size<New_N> {
		SAGE_ASSERT(std::in_range<New_N>(s.width) and std::in_range<New_N>(s.height));
		return { .width = static_cast<New_N>(s.width), .height = static_cast<New_N>(s.height) };
	}

};

}// sage::math
