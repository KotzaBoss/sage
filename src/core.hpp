#pragma once

#ifndef __cpp_lib_to_underlying
namespace std {

constexpr auto to_underlying(auto e) -> auto {
	return static_cast<std::underlying_type_t<decltype(e)>>(e);
}

}
#endif

namespace sage::inline core {

template<std::integral I>
consteval auto bits() -> size_t {
	return sizeof(I) * 8;
}

}// sage
