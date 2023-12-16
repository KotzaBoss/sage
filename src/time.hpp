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

template<Number Rep> using microseconds = std::chrono::duration<Rep, std::micro>;
template<Number Rep> using milliseconds = std::chrono::duration<Rep, std::milli>;

template<Duration D = std::chrono::milliseconds>
struct Tick {
	using Duration = D;
	using Clock = std::chrono::steady_clock;

private:
	Clock::time_point prev;

public:
	Tick()
		: prev{Clock::now()}
	{}

public:
	// Tick and return elapsed time since previous tick
	auto operator() () -> Duration {
		const auto now = Clock::now();
		return std::chrono::duration_cast<Duration>(now - std::exchange(prev, now));
	}

	auto current_time_point() const -> Clock::time_point {
		return prev;
	}
};

}// sage::time
