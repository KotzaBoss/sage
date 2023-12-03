#pragma once

#include "src/std.hpp"
#include "src/time.hpp"
#include "src/util.hpp"

namespace sage::inline perf {

struct Profiler {
	using Duration = std::chrono::microseconds;

public:
	static Profiler global;

public:
	struct Rendering {
		struct Batch {
			size_t max_quads;

			friend FMT_FORMATTER(Batch);
		};

		struct Result {
			std::optional<Batch> batch;
			uintmax_t draw_calls,
					  quads;

		public:
			Result()
				: draw_calls{0}
				, quads{0}
			{}

			Result(std::optional<Batch>&& batch)
				: batch{std::move(batch)}
				, draw_calls{0}
				, quads{0}
			{}

			Result(Result&& other)
				: batch{other.batch} // copy to not lose the information
				, draw_calls{std::exchange(other.draw_calls, 0)}
				, quads{std::exchange(other.quads, 0)}
			{}

		public:
			friend FMT_FORMATTER(Result);
		};

	private:
		std::string_view name;
		Result& result;
		std::function<void(Result&)> fn;

	public:
		template <std::invocable<Result&> Fn>
		constexpr Rendering(const std::string_view n, Result& r, Fn&& _fn)
			: name{n}
			, result{r}
			, fn{std::forward<Fn>(_fn)}
		{}

		~Rendering() {
			std::invoke(fn, result);
		}
	};

	struct Timer : Tick<Duration> {
		struct Result {
			std::string_view name;
			Duration duration;

			friend FMT_FORMATTER(Result);
		};

	private:
		std::string_view name;
		std::vector<Result>& results;

	public:
		constexpr Timer(const std::string_view n, std::vector<Result>& r)
			: Tick()
			, name{n}
			, results{r}
		{}

		~Timer() {
			results.emplace_back(name, std::invoke(*this));
		}
	};

public:
	using Results = util::Polymorphic_Array<
			std::vector<Timer::Result>,
			Rendering::Result
		>;

private:
	Results results;

public:
	Profiler() = default;
	Profiler(Rendering::Batch&& batch)
		: results{std::vector<Timer::Result>{}, std::move(batch)}
	{}

public:
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wvariadic-macros"

	#ifndef NDEBUG
	// Unecessarily dark, magic
	#define DETAIL_PROFILER_TIME_IMPL(_prof_, _name_, _func_, _line_) const auto timer_##_func_##_##_line_ = _prof_.time(_name_)
	#define DETAIL_PROFILER_TIME_FORWARD(_prof_, _name_, _func_, _line_) DETAIL_PROFILER_TIME_IMPL(_prof_, _name_, _func_, _line_)

	#define PROFILER_TIME(_prof_, _name_) DETAIL_PROFILER_TIME_FORWARD(_prof_, _name_, __func__, __LINE__)

	#else
	#define PROFILER_TIME(...) (void)0
	#endif

	#pragma GCC diagnostic pop

	[[nodiscard]]
	auto time(const std::string_view name) -> Timer {
		return {name, results.get<std::vector<Timer::Result>>()};
	}

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wvariadic-macros"

	#ifndef NDEBUG
	// Unecessarily dark, magic
	#define DETAIL_PROFILER_RENDERING_IMPL(_prof_, _name_, _func_, _line_, lambda...) const auto timer_##_func_##_##_line_ = _prof_.rendering(_name_, lambda)
	#define DETAIL_PROFILER_RENDERING_FORWARD(_prof_, _name_, _func_, _line_, lambda...) DETAIL_PROFILER_RENDERING_IMPL(_prof_, _name_, _func_, _line_, lambda)

	#define PROFILER_RENDERING(_prof_, _name_, lambda...) DETAIL_PROFILER_RENDERING_FORWARD(_prof_, _name_, __func__, __LINE__, lambda)

	#else
	#define PROFILER_RENDERING(...) (void)0
	#endif

	#pragma GCC diagnostic pop

	template <std::invocable<Rendering::Result&> Fn>
	[[nodiscard]]
	auto rendering(const std::string_view name, Fn&& fn) -> Rendering {
		return Rendering{ name, results.get<Rendering::Result>(), std::forward<Fn>(fn) };
	}

	[[nodiscard]]
	auto consume_results() -> Results {
		// Writing: return std::move(results); in one line does not work
		auto r = std::move(results);
		return r;
	}
};

inline Profiler Profiler::global;

}// sage::perf

template <>
FMT_FORMATTER(sage::perf::Profiler::Rendering::Batch) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::perf::Profiler::Rendering::Batch) {
		return fmt::format_to(ctx.out(), "Rendering::Batch: max_quads={};", obj.max_quads);
	}
};


template<>
FMT_FORMATTER(sage::perf::Profiler::Rendering::Result) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::perf::Profiler::Rendering::Result) {
		return fmt::format_to(ctx.out(),
				"Rendering::Result: batch={} quads={} draw_calls={};",
				obj.batch,
				obj.quads,
				obj.draw_calls
			);
	}
};

template<>
FMT_FORMATTER(sage::perf::Profiler::Timer::Result) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::perf::Profiler::Timer::Result) {
		return fmt::format_to(ctx.out(), "Timer::Result: name={} duration={};", obj.name, obj.duration);
	}
};

