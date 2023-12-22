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

template<Duration D = std::chrono::milliseconds>
struct Lifetime : time::Tick<D> {
	using Tick = time::Tick<D>;
	using Duration = D;
	using Clock = Tick::Clock;

private:
	Clock::time_point birth,	// Left it for debug, can preprocess it away if really needed.
					  death;

public:
	Lifetime(const Clock::duration life_expectancy)
		: Tick()
		, birth{Tick::current_time_point()}
		, death{Tick::current_time_point() + life_expectancy}
	{}

	auto is_dead(const Clock::time_point now = Clock::now()) const -> bool {
		return now > death;
	}

	auto is_alive(const Clock::time_point now = Clock::now()) const -> bool {
		return not is_dead(now);
	}
};

}// sage::time
