#pragma once

#include "std.hpp"

#include "repr.hpp"
#include "log.hpp"

namespace sage::inline util {

template <std::unsigned_integral I>
struct ID : std::optional<I> {
	using std::optional<I>::optional;

	constexpr ID(ID&& other)
		: std::optional<I>::optional{std::move(other)}
	{
		other.reset();
	}

	constexpr auto operator= (const ID&) -> ID& = default;
};


template <typename... Fns>
struct Overloaded : Fns... {
	using Fns::operator()...;
};

namespace string {
// Modified with ranges from: https://_stackoverflow.com/questions/216823/how-to-trim-an-stdstring
constexpr auto trim(std::string& s, const char to_trim = ' ') -> void {
	// Left trim
	s.erase(
			s.begin(),
			rg::find_if_not(s, [=] (const auto c) { return c == to_trim; })
		);
	// Right trim
	// Note the .base() of the reverse iterator
	s.erase(
			rg::find_if_not(s | vw::reverse, [=] (const auto c) { return c == to_trim; })
				.base(),
			s.end()
		);
}
} // sage::util::string

constexpr auto truth(const auto& x) -> bool {
	return x;
}

template<std::integral I>
constexpr auto bits = sizeof(I) * 8;

// Use with care because there may still be a race.
// Prefer using it when the atomic is strictly encapsulated/controlled,
// and externals only read it (see window::Base).
//
// TL;DR
//
// class Complicated {
// private:
//		std::atomic<Data> _data;
//
// public:
//      auto work() -> void {
//			atomic_delta_store(_data, [] (auto d) {
//					d.some_field = 123;
//					return d;
//				});
//      }
//
//      auto data() const -> Data {
//			return _data.load();
//      }
// };
template <typename T>
auto atomic_delta_store(std::atomic<T>& a, const std::invocable<T&> auto& delta) -> std::atomic<T>& {
	// A bit excessive but this way the caller has the more elegant code.
	// CAUTION: As of GCC 13 a lambda [](auto) can be passed as a [](auto & ):
	// atomic_delta_store(a, [] (auto x) { ... });
	// atomic_delta_store(a, [] (auto& x) { ... });
	// Both compile but i have no interest in testing whether it is correct.
	// It also issues a "x unused" warning so its not good to begin with.
	auto t = a.load();
	delta(t);
	a.store(t);
	return a;
}

#define LOCK_GUARD(mutex) const auto _ = std::lock_guard{m}

// Inspired by Herb Sutter: https://stackoverflow.com/questions/60522330/how-does-herb-sutters-monitor-class-work
template<typename T>
struct Monitor {
	using Object = T;
	using Object_Ref = T&;

protected:
	mutable Object obj;
	mutable std::mutex m;

public:
	Monitor(Monitor&& other)
		: obj{std::move(other.obj)}
	{}

	template<typename... Args>
	Monitor(Args&&... args)
		: obj(std::forward<Args>(args)...)
	{}

public:
	auto invoke(std::invocable<Object_Ref> auto fn) const -> auto {
		LOCK_GUARD(m);
		return std::invoke(fn, obj);
	}

	// For when we know our Object has its own safety.
	// For example std::promise::set_value is already thread safe so no need to lock twice.
	auto unsafe_invoke(std::invocable<Object_Ref> auto fn) const -> auto {
		return std::invoke(fn, obj);
	}

	template <typename Obj>
		// Could not really figure out how to constrain it without this monstrosity.
		// std::assignable_from whines about the LHS not being a lvalue.
		requires requires (Obj o, Object dest) { dest = o; }
	auto store(Obj&& o) const -> void {
		LOCK_GUARD(m);
		obj = std::forward<Obj>(o);
	}

	auto store(std::invocable<Object_Ref> auto fn) -> void {
		LOCK_GUARD(m);
		fn(obj);
	}

	auto load() const -> Object {
		if constexpr (not std::is_trivially_copyable_v<T>)
			SAGE_LOG_DEBUG("Guarded object of Monitor is not trivially copyable, consider using the load(invocable) overload");

		LOCK_GUARD(m);
		return obj;
	}

	auto load(const std::invocable<const Object_Ref> auto fn) -> void {
		LOCK_GUARD(m);
		fn(obj);
	}

	auto release() const -> Object {
		LOCK_GUARD(m);
		return std::exchange(obj, Object{});
	}
};

template <typename Needle, typename... Haystack>
concept same_as_any = (std::same_as<Needle, Haystack> or ...);

// Needed to be able to define Unique as Unique<typename...> and not Unique<typename Needle, typename... Haystack>
namespace detail {
	template <typename...>
	inline constexpr auto unique = true;

	template <typename T, typename... Rest>
	inline constexpr auto unique<T, Rest...> = (not same_as_any<T, Rest> and ...) and unique<Rest...>;
}

template <typename... T>
concept Unique = detail::unique<T...>;

namespace type {

using Real_Name_Ptr = std::unique_ptr<char, decltype([] (auto ptr) { std::free(ptr); })>;

template <typename T>
constexpr auto real_name() -> Real_Name_Ptr {
	auto _real_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
	SAGE_ASSERT(_real_name);
	return Real_Name_Ptr{ _real_name };
}

constexpr auto real_name(auto&& x) -> Real_Name_Ptr {
	return real_name<decltype(x)>();
}

template <typename X>
struct Counter {
	using Type = X;

public:
	size_t value = 0;

public:
	constexpr auto operator++ () -> Counter& {
		++value;
		return *this;
	}

	constexpr auto operator* () const -> size_t {
		return value;
	}

	constexpr auto operator== (const std::unsigned_integral auto& other) const -> bool {
		return value == other;
	}
};

// See below (outside of namespace sage) for the specialization of tuple_size, to make
// gcc happy...
template <typename... Ts>
struct Counters : std::tuple<Counter<Ts>...> {

	template <same_as_any<Ts...> X>
	constexpr auto increment() -> void {
		++std::get<Counter<X>>(*this);
	}

	template <typename X>
	constexpr auto count() -> size_t {
		if constexpr (same_as_any<X, Ts...>)
			return *std::get<Counter<X>>(*this);
		else
			return 0;
	}
};

template <typename... Ts>
	requires Unique<Ts...>
struct Set {
	Set() = delete;

	using Set_Counters = type::Counters<Ts...>;

	// Count occurences of Ts... in Xs...
	// Syntactically simplified examples:
	//
	//		Set<int, float>::count<int>()				== tuple(1, 0);
	//		Set<int, float>::count<float>()				== tuple(0, 1);
	//		Set<int, float>::count<float, int, float>() == tuple(1, 2);
	//
	// See util.test.cpp for more extensive calls
	template <typename... Xs>
		requires (sizeof...(Xs) > 0)
	static consteval auto count() -> type::Counters<Ts...> {
		auto counters = type::Counters<Ts...>{};
		(increment_counter_if_exists<Xs>(counters), ...);
		return counters;
	}

	template <typename... Xs>
		requires (sizeof...(Xs) <= 1)
	static consteval auto contains() -> bool {
		return (same_as_any<Xs, Ts...> or ...);
	}

private:
	template <typename X>
	static consteval auto increment_counter_if_exists(type::Counters<Ts...>& counters) -> void {
		if constexpr (same_as_any<X, Ts...>)
			counters.template increment<X>();
	}

};

}// type

template <typename... Ts>
	requires Unique<Ts...>
struct Polymorphic_Array {
	using Storage = std::tuple<Ts...>;

protected:
	Storage storage;

public:
	Polymorphic_Array(same_as_any<Ts...> auto&&... ts)
		: storage{std::make_tuple(std::move(ts)...)}
	{}

public:
	constexpr auto size() const -> size_t {
		return sizeof...(Ts);
	}

	auto front() -> std::tuple_element_t<0, Storage>& {
		return std::get<0>(storage);
	}

	template <typename Fn>
		requires ((std::invocable<Fn, Ts&> and ...) or (std::invocable<Fn, const Ts&> and ...))
	auto apply(Fn&& fn) -> void {
		std::apply(
				[&] (auto&... t) {
					(std::invoke(std::forward<Fn>(fn), t), ...);
				},
				storage
			);
	}

	#pragma message "FIXME: Make constness work for Polymorphic_* containers. Why app chooses this when there is no const?"
	//template <typename Fn>
	//	requires (std::invocable<Fn, const Ts&> and ...)
	//auto apply(Fn&& fn) const -> void {
	//	std::apply(
	//			[&] (const auto&... t) {
	//				(std::invoke(std::forward<Fn>(fn), t), ...);
	//			},
	//			storage
	//		);
	//}
};

template <typename... Ts>
	requires Unique<Ts...>
struct Polymorphic_Storage {
	template <typename Q>
	using Vector = std::vector<Q>;

	using Storage = std::tuple<Vector<Ts>...>;

protected:
	Storage storage;

public:
	Polymorphic_Storage() = default;

	// CAUTION: Vectors must be in the order that Ts are declared
	//
	// using S = Polymorphic_Storage<int, string>
	// s = S{ {1,2,3}, {"4"s, "5"s} };  // Ok
	// s = S{ {"4"s, "5"s}, {1,2,3} };  // Error, propably: candidate Polymorphic_Storage({brace enclosed initializer list})
	//
	constexpr Polymorphic_Storage(Vector<Ts>&&... vs) {
		((std::get<std::decay_t<decltype(vs)>>(storage) = std::move(vs)), ...);
	}

public:
	auto store(same_as_any<Ts...> auto&& x) -> void {
		using Type = std::decay_t<decltype(x)>;
		std::get<Vector<Type>>(storage)
			.push_back(std::move(x));
	}

	template <same_as_any<Ts...> T>
	auto get() const -> const Vector<T>& {
		return std::get<T>(storage);
	}

	template <same_as_any<Ts...> T>
	auto get() -> Vector<T>& {
		return std::get<T>(storage);
	}

	auto size() const -> size_t {
		return std::apply(
				[] (const auto&... vec) {
					return (vec.size() + ...);
				},
				storage
			);
	}

	auto front() -> std::tuple_element_t<0, Storage>::reference {
		return std::get<0>(storage).front();
	}

											// Apply
											//

	template <typename Fn>
		requires ((std::invocable<Fn, Ts&> and ...) or (std::invocable<Fn, const Ts&> and ...))
	auto apply(Fn&& fn) -> void {
		std::apply(
				[&] (auto&... vec) {
					(rg::for_each(vec, [&] (auto& t) { std::invoke(std::forward<Fn>(fn), t); }), ...);
				},
				storage
			);
	}

	#pragma message "FIXME: See pragma in Polymorphic_Array"
	//template <typename Fn>
	//	requires (std::invocable<Fn, const Ts&> and ...)
	//auto apply(Fn&& fn) const -> void {
	//	std::apply(
	//			[&] (const auto&... vec) {
	//				(rg::for_each(vec, [&] (const auto& t) { std::invoke(std::forward<Fn>(fn), t); }), ...);
	//			},
	//			storage
	//		);
	//}

	template <typename Fn>
		requires ((std::invocable<Fn, Vector<Ts>&> and ...) or (std::invocable<Fn, const Vector<Ts>&> and ...))
	auto apply_group(Fn&& fn) -> void {
		std::apply(
				[&] (auto&... vec) {
					(std::invoke(std::forward<Fn>(fn), vec), ...);
				},
				storage
			);
	}

	#pragma message "FIXME: See pragma in Polymorphic_Array"
	//template <typename Fn>
	//	requires (std::invocable<Fn, const Vector<Ts>&> and ...)
	//auto apply_group(Fn&& fn) const -> void {
	//	std::apply(
	//			[&] (const auto&... vec) {
	//				(std::invoke(std::forward<Fn>(fn), vec), ...);
	//			},
	//			storage
	//		);
	//}

public:
	friend REPR_DEF_FMT(Polymorphic_Storage<Ts...>);
	friend FMT_FORMATTER(Polymorphic_Storage<Ts...>);
};

}// util

template <typename... Ts>
struct std::tuple_size<sage::util::type::Counters<Ts...>>
	: std::tuple_size<std::tuple<sage::util::type::Counter<Ts>...>>
{};

template <typename T>
FMT_FORMATTER(sage::util::type::Counter<T>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::util::type::Counter<T>) {
		return fmt::format_to(ctx.out(), "type::Counter<{}>: {};", sage::util::type::real_name<T>().get(), *obj);
	}
};

template <>
FMT_FORMATTER(sage::util::type::Real_Name_Ptr) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::util::type::Real_Name_Ptr) {
		return fmt::format_to(ctx.out(), "type::Real_Name_Ptr: {};", obj.get());
	}
};

template <typename... Ts>
FMT_FORMATTER(sage::util::Polymorphic_Array<Ts...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::util::Polymorphic_Array<Ts...>) {
		fmt::format_to(ctx.out(), "util::Polymorphic_Array: ");
		// Dark magic
		#pragma message "FIXME: Use const auto in lambda when Polymorphic_* have constness"
		const_cast<std::remove_const_t<std::decay_t<decltype(obj)>>&>(obj).apply([&] (auto& x) { fmt::format_to(ctx.out(), "\n\t{}", x); });
		return fmt::format_to(ctx.out(), "\n\t;");
	}
};


template <typename... Ts>
FMT_FORMATTER(sage::util::Polymorphic_Storage<Ts...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::util::Polymorphic_Storage<Ts...>) {
		fmt::format_to(ctx.out(), "util::Polymorphic_Array: ");
		#pragma message "FIXME: Use const auto in lambda when Polymorphic_* have constness"
		const_cast<std::remove_const_t<std::decay_t<decltype(obj)>>&>(obj).apply([&] (auto& x) { fmt::format_to(ctx.out(), "\n\t{}", x); });
		return fmt::format_to(ctx.out(), "\n\t;");
	}
};
