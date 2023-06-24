#pragma once

#include "std.hpp"

namespace sage::inline util {

template <typename Needle, typename... Haystack>
concept same_as_any = (... or std::same_as<Needle, Haystack>);

template <typename... Ts>
struct Polymorphic_Tuple_Storage {
	template <typename Q>
	using Vector = std::vector<Q>;

	using Storage = std::tuple<Vector<Ts>...>;

protected:
	Storage storage;

public:
	Polymorphic_Tuple_Storage() = default;

	Polymorphic_Tuple_Storage(same_as_any<Ts...> auto&&... xs) {
			(_store(std::move(xs)), ...);
		}


public:
	auto store(same_as_any<Ts...> auto&& x) -> void {
		_store(std::move(x));
	}

	template <typename Fn>
		requires (... and std::invocable<Fn, Ts&>)
	auto apply(Fn&& fn) -> void {
		std::apply(
				[&] (auto&... vec) {
					(rg::for_each(vec, [&] (auto& t) { fn(t); }), ...);
				},
				storage
			);
	}

private:
	auto _store(same_as_any<Ts...> auto&& x) -> void {
		using Type = std::decay_t<decltype(x)>;
		std::get<Vector<Type>>(storage)
			.push_back(std::move(x));
	}

};

}
