#pragma once

#include "std.hpp"
#include "time.hpp"

namespace sage::inline perf {

struct Profiler {
	using Duration = std::chrono::microseconds;

public:
	struct Result {
		std::string_view name;
		Duration duration;
	};

	using Results = std::vector<Result>;

	struct Timer : Tick<Duration> {
	private:
		std::string_view name;
		Profiler& profiler;

	public:
		constexpr Timer(const std::string_view n, Profiler& p)
			: Tick()
			, name{n}
			, profiler{p}
		{}

		~Timer() {
			profiler._results.emplace_back(name, std::invoke(*this));
		}
	};

private:
	Results _results;

public:
	#ifndef NDEBUG
	// Unecessarily dark, magic
	#define DETAIL_PROFILER_TIME_IMPL(_prof_, _name_, _func_, _line_) const auto timer_##_func_##_##_line_ = _prof_.time(_name_)
	#define DETAIL_PROFILER_TIME_FORWARD(_prof_, _name_, _func_, _line_) DETAIL_PROFILER_TIME_IMPL(_prof_, _name_, _func_, _line_)

	#define PROFILER_TIME(_prof_, _name_) DETAIL_PROFILER_TIME_FORWARD(_prof_, _name_, __func__, __LINE__)

	#else
	#define PROFILER_TIME(...) (void)0
	#endif

	[[nodiscard]]
	auto time(const std::string_view name) -> Timer {
		return {name, *this};
	}

	[[nodiscard]]
	auto consume_results() -> Results {
		// Writing: return std::move(_results); in one line does not work
		const auto r = std::move(_results);
		return r;
	}
};

}// sage::perf
