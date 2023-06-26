#pragma once

#include "std.hpp"

#include "repr.hpp"

namespace sage::inline util {

template <typename Needle, typename... Haystack>
concept same_as_any = (std::same_as<Needle, Haystack> or ...);

template <typename... Ts>
struct Polymorphic_Storage {
	template <typename Q>
	using Vector = std::vector<Q>;

	using Storage = std::tuple<Vector<Ts>...>;

protected:
	Storage storage;

public:
	Polymorphic_Storage() = default;

	Polymorphic_Storage(same_as_any<Ts...> auto&&... xs) {
			(_store(std::move(xs)), ...);
		}


public:
	auto store(same_as_any<Ts...> auto&& x) -> void {
		_store(std::move(x));
	}

	template <same_as_any<Ts...> T> auto get() const	-> const Vector<T>&	{ return std::get<T>(storage); }
	template <same_as_any<Ts...> T> auto get()			-> Vector<T>&		{ return std::get<T>(storage); }

	auto size() const -> size_t {
		return std::apply(
				[] (const auto&... vec) {
					return (vec.size() + ...);
				},
				storage
			);
	}

	template <typename Fn>
		requires (std::invocable<Fn, Ts&> and ...)
	auto apply(const Fn& fn) -> void {
		std::apply(
				[&] (auto&... vec) {
					(rg::for_each(vec, [&] (auto& t) { std::invoke(fn, t); }), ...);
				},
				storage
			);
	}

	// FIXME: Had to rename the const overload because the fmt::formatter doesnt work otherwise. fix?
	template <typename Fn>
		requires (std::invocable<Fn, const Ts&> and ...)
	auto const_apply(const Fn& fn) const -> void {
		std::apply(
				[&] (const auto&... vec) {
					(rg::for_each(vec, [&] (const auto& t) { std::invoke(fn, t); }), ...);
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

public:
	friend REPR_DEF_FMT(Polymorphic_Storage<Ts...>);
	friend FMT_FORMATTER(Polymorphic_Storage<Ts...>);
};

}// util


