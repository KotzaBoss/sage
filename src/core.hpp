#pragma once

#include "std.hpp"

#ifndef __cpp_lib_to_underlying
namespace std {

constexpr auto to_underlying(auto e) -> auto {
	return static_cast<std::underlying_type_t<decltype(e)>>(e);
}

}
#endif

#define SAGE_ASSERT(cond) assert(cond)
#define SAGE_ASSERT_MSG(cond, msg) assert((msg, cond))

namespace sage::inline core {

template<std::integral I>
consteval auto bits() -> size_t {
	return sizeof(I) * 8;
}

}// sage
