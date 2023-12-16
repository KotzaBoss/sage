#pragma once

#include "src/std.hpp"
#include "src/repr.hpp"
#include "src/time.hpp"
#include "src/util.hpp"
#include "imgui.h"

namespace sage::inline perf {

namespace target {
	namespace fps {
		constexpr auto _30 = 1000ms / 30;
		constexpr auto _60 = 1000ms / 60;
		constexpr auto _144 = 1000ms / 144;
	}

	namespace time_point {
		using Time_Point = std::chrono::steady_clock::time_point;

		constexpr auto _30	(const Time_Point& start) -> Time_Point { return start + fps::_30;	}
		constexpr auto _60	(const Time_Point& start) -> Time_Point { return start + fps::_60;	}
		constexpr auto _144	(const Time_Point& start) -> Time_Point { return start + fps::_144;	}
	}

	constexpr auto legend() -> std::string {
		// This is ugly x( but it works x)
		return fmt::format(
R"(FPS:
	 30 -> {:>5}
	 60 -> {:>5}
	144 -> {:>5}
)",
				fps::_30, fps::_60, fps::_144
			);
	}
} //targets


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

			Result(const Result&) = default;
			Result(Result&& other)
				: batch{other.batch} // copy to not lose the information
				, draw_calls{std::exchange(other.draw_calls, 0)}
				, quads{std::exchange(other.quads, 0)}
			{}

			auto operator= (Result&& other) -> Result& {
				draw_calls = std::exchange(other.draw_calls, 0);
				quads = std::exchange(other.quads, 0);

				return *this;
			}

		public:
			friend FMT_FORMATTER(Result);
		};

	private:
		Result& result;
		std::function<void(Result&)> fn;

	public:
		template <std::invocable<Result&> Fn>
		constexpr Rendering(Result& r, Fn&& _fn)
			: result{r}
			, fn{std::forward<Fn>(_fn)}
		{}

		~Rendering() {
			std::invoke(fn, result);
		}
	};

	struct Timer : Tick<Duration> {
		struct Result {
			Duration duration;

			friend FMT_FORMATTER(Result);
		};

	private:
		Result& results;

	public:
		constexpr Timer(Result& r)
			: Tick()
			, results{r}
		{}

		~Timer() {
			results = {std::invoke(*this)};
		}
	};

public:
	struct Timer_Result_Pair {
		std::string_view name;
		Timer::Result result;

		friend FMT_FORMATTER(Timer_Result_Pair);
	};
	using Timer_Results = std::vector<Timer_Result_Pair>;

	using Results = util::Polymorphic_Array<
			Timer_Results,
			Rendering::Result
		>;

private:
	Results results;

public:
	Profiler() = default;
	Profiler(Rendering::Batch&& batch, const size_t timer_result_capacity = 100)
		: results{
			Timer_Results{},
			Rendering::Result{std::move(batch)}
		}
	{
		results.get<Timer_Results>().reserve(timer_result_capacity);
	}

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
		auto& timer_results = results.get<Timer_Results>();

		SAGE_ASSERT(timer_results.size() < timer_results.capacity(),
				"Make sure the profiling results are consumed at some point and if you need more increase the `timer_results_capacity` hint in the Profiler constructor", timer_results.size(), timer_results.capacity());

		timer_results.emplace_back(name);
		auto& res = timer_results.back().result;
		return { res };
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
		return Rendering{ results.get<Rendering::Result>(), std::forward<Fn>(fn) };
	}

	[[nodiscard]]
	auto consume_results() -> Results {
		// Writing: return std::move(results); in one line does not work
		auto r = results;

		results.get<Timer_Results>().clear();
		results.get<Rendering::Result>() = Rendering::Result();

		return r;
	}
};

inline Profiler Profiler::global;

}// sage::perf

template <>
FMT_FORMATTER(sage::perf::Profiler::Rendering::Batch) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::perf::Profiler::Rendering::Batch) {
		return fmt::format_to(ctx.out(), "max_quads={}", obj.max_quads);
	}
};


template<>
FMT_FORMATTER(sage::perf::Profiler::Rendering::Result) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::perf::Profiler::Rendering::Result) {
		return fmt::format_to(ctx.out(),
				"batch{{{}}} quads={} draw_calls={}",
				// TODO: Make a specialization that is shorter than fmt's optional(...)
				std::invoke([&] {
						if (obj.batch.has_value())
							return fmt::format("{}", *obj.batch);
						else
							return "Unspecified"s;
					}),
				obj.quads,
				obj.draw_calls
			);
	}
};

template<>
FMT_FORMATTER(sage::perf::Profiler::Timer::Result) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::perf::Profiler::Timer::Result) {
		return fmt::format_to(ctx.out(), "duration={};", obj.duration);
	}
};

template<>
FMT_FORMATTER(sage::perf::Profiler::Timer_Result_Pair) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::perf::Profiler::Timer_Result_Pair) {
		return fmt::format_to(ctx.out(), "(name={:?} result={})", obj.name, obj.result);
	}
};

template<>
FMT_FORMATTER(sage::perf::Profiler::Results) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::perf::Profiler::Results) {
		using Profiler = sage::perf::Profiler;

		const auto& timer_results = obj.get<Profiler::Timer_Results>();

		const auto total = rg::fold_left(
				timer_results,
				Profiler::Duration{},
				[] (const auto& acc, const auto& pair) {
					return acc + pair.result.duration;
				}
			);

		constexpr auto format_line = "{:15} {:>10} {:5.1f}\n";

		fmt::format_to(ctx.out(), "Profiler results:\n");
		fmt::format_to(ctx.out(), format_line, "Frame", total, 100.f);

		for (const auto& pair : timer_results) {
			using Float_Duration = std::chrono::duration<float, typename Profiler::Duration::period>;
			const auto percentage = (Float_Duration{pair.result.duration} / total) * 100;
			fmt::format_to(ctx.out(), format_line,
					pair.name,
					pair.result.duration,
					percentage
				);
		}

		const auto& rendering_result = obj.get<Profiler::Rendering::Result>();
		fmt::format_to(ctx.out(), "\nRendering {}", rendering_result);


		return fmt::format_to(ctx.out(), "\n=============================\nLegend\n{}", sage::perf::target::legend());
	}
};

namespace sage::perf {

}// sage::perf
