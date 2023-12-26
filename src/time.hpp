#pragma once

#include "src/std.hpp"
#include "src/util.hpp"

namespace sage::inline time {

template<typename D>
concept Duration = type::Any<D,
		std::chrono::milliseconds,
		std::chrono::microseconds
	>
	;

constexpr auto delta_coefficient(const Duration auto dt) -> float {
	return std::chrono::duration<float, std::chrono::seconds::period>{dt}.count();
}

template<Duration D = std::chrono::milliseconds>
struct Tick {
	using Duration = D;
	using Clock = std::chrono::steady_clock;

protected:
	Clock::time_point current;

public:
	Tick()
		: current{Clock::now()}
	{}

public:
	// Tick and return elapsed time since previous tick
	auto operator() () -> Duration {
		const auto now = Clock::now();
		return std::chrono::duration_cast<Duration>(now - std::exchange(current, now));
	}

	auto current_time_point() const -> Clock::time_point {
		return current;
	}
};

template<Duration D = std::chrono::milliseconds>
struct Lifetime {
	using Duration = D;
	using Clock = std::chrono::steady_clock;

private:
	Clock::time_point birth,	// Left it for debug, can preprocess it away if really needed.
					  current,
					  death;

public:
	Lifetime(const Clock::duration life_expectancy)
		: birth{Clock::now()}
		, current{birth}
		, death{birth + life_expectancy}
	{}

	auto tick(const Clock::duration dt) -> void {
		current += dt;
	}

	auto is_dead() const -> bool {
		return current > death;
	}

	auto is_alive() const -> bool {
		return not is_dead();
	}
};

}// sage::time
