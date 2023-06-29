#pragma once

#include "std.hpp"

namespace sage::graphics {

template <typename T>
concept Context =
	requires (T t) {
		{ t.setup() } -> std::same_as<void>;
		{ t.swap_buffers() } -> std::same_as<void>;
	}
	;

}// sage::graphics

