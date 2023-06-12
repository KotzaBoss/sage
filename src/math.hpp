#pragma once

#include "std.hpp"

namespace sage::inline math {

template<typename T>
concept Number = std::integral<T> or std::floating_point<T>;

template<Number N = size_t>
struct Size {
	N width, height;

	friend auto operator<< (std::ostream& o, const Size& s) -> std::ostream& {
		return o << "Size: width=" << s.width << " height=" << s.height << ';';
	}
};

}// sage::math
