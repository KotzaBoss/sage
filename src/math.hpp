#pragma once

#include "src/std.hpp"
#include "src/util.hpp"
#include "src/log.hpp"

#include "src/repr.hpp"

#include "src/glm.hpp"

namespace sage::inline math {

template<typename T>
	requires type::Any<T,
			glm::vec2,
			glm::vec3,
			glm::vec4,
			glm::mat4
		>
constexpr auto identity = T{1.f};

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

template <Number N>
constexpr auto in_range(const N x, const N min, const N max) -> bool {
	return min <= x and x <= max;
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

namespace random {

inline auto dev = std::random_device{};

auto engine() -> auto {
	return std::mt19937{dev()};
}

template<Number N>
constexpr auto in_range(const N min = 0, const N max = std::numeric_limits<N>::max()) -> N {
	// Things like this make people hate cpp...
	// Why not have a random(min, max) function that has the optimal implementation for
	// the current platform?
	auto eng = engine();
	if constexpr (std::floating_point<N>)
		return std::uniform_real_distribution{min, max}(eng);
	else
		return std::uniform_int_distribution{min, max}(eng);
}

template <Number N>
constexpr auto non_negative(const N max) -> N {
	return in_range(0, max);
}

template<rg::view V>
	requires Number<rg::range_value_t<V>>
constexpr auto index(const V& v) -> size_t {
	return in_range(0ul, clamp_low(rg::size(v) - 1, 0));
}

constexpr auto normalized() -> float {
	return in_range(0.f, 1.f);
}

template <Number... Ns>
	requires type::All<Ns...>
constexpr auto between(const Ns&... _ns) -> auto {
	const auto ns = {_ns...};

	using N = type::Front<Ns...>;
	auto out = std::array<N, 1>{};

	rg::sample(ns, out.begin(), 1, engine());

	return out[0];
}

namespace toggle {
constexpr auto any()		-> bool { return random::in_range(1, 100); }
constexpr auto often()		-> bool { return math::in_range(random::in_range(1, 100), 1, 75); }
constexpr auto uncommon()	-> bool { return math::in_range(random::in_range(1, 100), 1, 50); }
constexpr auto rare()		-> bool { return math::in_range(random::in_range(1, 100), 1, 25); }
}// random::toggle

}// random

}// sage::math

template <sage::Number N>
FMT_FORMATTER(sage::Size<N>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::Size<N>) {
		return fmt::format_to(ctx.out(), "Size: width={:4} height={:4};", obj.width, obj.height);
	}
};
