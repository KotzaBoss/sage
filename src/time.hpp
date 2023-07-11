#pragma once

#include "std.hpp"

namespace sage::inline time {

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
	auto operator() () -> std::chrono::milliseconds {
		const auto now = Clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now - std::exchange(prev, now));
	}

	auto current_time_point() const -> Clock::time_point {
		return prev;
	}
};

}// sage::time
