#pragma once

#include "src/std.hpp"

namespace sage::inline time {

template<typename Duration = std::chrono::milliseconds>
	requires type::Any<Duration,
			std::chrono::milliseconds,
			std::chrono::microseconds
		>
struct Tick {
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
