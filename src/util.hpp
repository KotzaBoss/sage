#pragma once

#include "src/std.hpp"

#include "src/repr.hpp"
#include "src/log.hpp"

namespace sage::inline util {

constexpr auto toogle_if(const bool x, const bool cond) -> bool {
	return x ^ cond;
}

struct ID : std::optional<uint32_t> {
	using Rep = uint32_t;
	using std::optional<Rep>::optional;

	constexpr ID(const ID&) = default;

	constexpr ID(ID&& other)
		: std::optional<Rep>::optional{std::move(other)}
	{
		other.reset();
	}

	constexpr auto operator= (const ID&) -> ID& = default;

	constexpr auto operator<=> (const ID&) const = default;
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

template <typename T>
concept Boolean_Testable =
	requires (T&& t) {
		{ static_cast<bool>(std::forward<T>(t)) } -> std::same_as<bool>;
	}
	;

template <Boolean_Testable T>
constexpr auto truth(T&& x) -> bool {
	return static_cast<bool>(std::forward<T>(x));
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

#define LOCK_GUARD(_mutex_) const auto _ = std::lock_guard{_mutex_}

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

namespace type {

inline namespace comp {

template <typename Needle, typename... Haystack>
concept Any = (std::same_as<Needle, Haystack> or ...);

namespace detail {
template <typename Needle, typename... Haystack>
concept All = (std::same_as<Needle, Haystack> and ...);

template <typename...>
inline constexpr auto all = true;

template <typename T, typename... Rest>
inline constexpr auto all<T, Rest...> = All<T, Rest...> and all<Rest...>;
}// detail

template <typename... T>
concept All = detail::all<T...>;

template <typename Needle, typename... Haystack>
concept Not_In = not Any<Needle, Haystack...>;

// Needed to be able to define Unique as Unique<typename...> and not Unique<typename Needle, typename... Haystack>
namespace detail {
template <typename...>
inline constexpr auto unique = true;

template <typename T, typename... Rest>
inline constexpr auto unique<T, Rest...> = Not_In<T, Rest...> and unique<Rest...>;
}

template <typename... T>
concept Unique = detail::unique<T...>;

template <size_t I, typename... Ts>
	requires (sizeof...(Ts) > 0)
using At = std::tuple_element_t<I, std::tuple<Ts...>>;

template <typename... Ts>
using Front = At<0, Ts...>;

template <typename... Ts>
using Back = At<sizeof...(Ts) - 1, Ts...>;

}// type::comp

using Real_Name_Ptr = std::unique_ptr<char, decltype([] (auto ptr) { std::free(ptr); })>;

template <typename T>
constexpr auto real_name() -> Real_Name_Ptr {
	auto _real_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
	SAGE_ASSERT(_real_name);
	return Real_Name_Ptr{ _real_name };
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

// See below (outside of namespace sage) for the specialization of tuple_size, to make gcc happy...
template <typename... Ts>
struct Counters : std::tuple<Counter<Ts>...> {

	template <Any<Ts...> X>
	constexpr auto increment() -> void {
		++std::get<Counter<X>>(*this);
	}

	template <typename X>
	constexpr auto count() -> size_t {
		if constexpr (Any<X, Ts...>)
			return *std::get<Counter<X>>(*this);
		else
			return 0;
	}
};

}// sage::util::type
}// sage::util

template <typename... Ts>
struct std::tuple_size<sage::util::type::Counters<Ts...>>
	: std::tuple_size<std::tuple<sage::util::type::Counter<Ts>...>>
{};

namespace sage::inline util {
namespace type {

template <typename... Ts>
	requires Unique<Ts...>
struct Set {
	using Index_Sequence = std::index_sequence_for<Ts...>;

	template <size_t I>
	using At = type::At<I, Ts...>;

	Set() = delete;

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
		return (Any<Xs, Ts...> or ...);
	}

	static consteval auto size() -> size_t {
		return sizeof...(Ts);
	}

private:
	template <typename X>
	static consteval auto increment_counter_if_exists(type::Counters<Ts...>& counters) -> void {
		if constexpr (Any<X, Ts...>)
			counters.template increment<X>();
	}

};

}// type
}// sage::util

namespace sage::inline util {

// The fmtlib is a pain to deal with if Polymorphic_Storage subclasses Polymorphic_Array
// so just inherit both from tuple.
template <typename... Ts>
using Polymorphic_Container_Base = std::tuple<Ts...>;

template <typename... Ts>
	requires type::Unique<Ts...>
struct Polymorphic_Array : Polymorphic_Container_Base<Ts...> {
	using Base = Polymorphic_Container_Base<Ts...>;
	using Base::Base;

public:
	constexpr auto size() const -> size_t {
		return std::tuple_size_v<Base>;
	}

	constexpr auto front() -> std::tuple_element_t<0, Base>& {
		return std::get<0>(*this);
	}

	#if __cplusplus >= 202302L	// >C++23
	// TODO: Deduce this to add const-correctness to Polymorphic_Containers.
	#endif
	// TODO: add check std>=23 todo deduce this
	template <typename Fn>
		requires ((std::invocable<Fn, Ts&> and ...) or (std::invocable<Fn, const Ts&> and ...))
	constexpr auto apply(Fn&& fn) -> decltype(auto) {
		return std::apply(
				[&] (auto&... t) {
					return (std::invoke(std::forward<Fn>(fn), t), ...);
				},
				*this
			);
	}

	// See above because this is ugly
	template <typename Fn>
		requires (std::invocable<Fn, const Ts&> and ...)
	constexpr auto const_apply(Fn&& fn) const -> decltype(auto) {
		return std::apply(
				[&] (const auto&... t) {
					return (std::invoke(std::forward<Fn>(fn), t), ...);
				},
				*this
			);
	}

	template <type::Any<Ts...> T>
	constexpr auto get() -> T& {
		return std::get<T>(*this);
	}

	template <type::Any<Ts...> T>
	constexpr auto get() const -> const T& {
		return std::get<T>(*this);
	}
};

template <typename... Ts>
Polymorphic_Array(Ts&&...) -> Polymorphic_Array<Ts...>;

}// sage::util

template <typename... Ts>
struct std::tuple_size<sage::util::Polymorphic_Array<Ts...>>
	: std::tuple_size<typename sage::util::Polymorphic_Array<Ts...>::Base>
{};

template <size_t I, typename... Ts>
struct std::tuple_element<I, sage::util::Polymorphic_Array<Ts...>>
	: std::tuple_element<I, typename sage::util::Polymorphic_Array<Ts...>::Base>
{};


namespace sage::inline util {

template <typename... Ts>
	requires type::Unique<Ts...>
struct Polymorphic_Storage : Polymorphic_Container_Base<std::vector<Ts>...> {
	using Types = type::Set<Ts...>;
	template <typename Q>
	using Vector = std::vector<Q>;
	template <typename Q>
	using Reference = std::vector<Q>&;
	using Base = Polymorphic_Container_Base<Vector<Ts>...>;
	using Base::Base;

public:
	constexpr
	Polymorphic_Storage(const size_t size) {
		resize(size);
	}

public:
	constexpr auto size() const -> size_t {
		return std::apply(
				[] (const auto&... vec) {
					return (vec.size() + ...);
				},
				*this
			);
	}

	constexpr auto front() -> std::tuple_element_t<0, Base>& {
		return std::get<0>(*this).front();
	}

	template <typename Fn>
		requires ((std::invocable<Fn, Ts&> and ...) or (std::invocable<Fn, const Ts&> and ...))
	constexpr auto apply(Fn&& fn) -> decltype(auto) {
		return std::apply(
				[&] (auto&... vec) {
					return (rg::for_each(vec, [&] (auto& t) { return std::invoke(std::forward<Fn>(fn), t); }), ...);
				},
				*this
			);
	}

	template <typename Fn>
		requires ((std::invocable<Fn, Vector<Ts>&> and ...) or (std::invocable<Fn, const Vector<Ts>&> and ...))
	constexpr auto apply_group(Fn&& fn) -> decltype(auto) {
		return std::apply(
				[&] (auto&... vec) {
					return (std::invoke(std::forward<Fn>(fn), vec), ...);
				},
				*this
			);
	}

	constexpr auto resize(const size_t cap) -> void {
		apply_group([&] (auto& vec) {
				vec.resize(cap);
			});
	}

};

template <typename... Ts>
Polymorphic_Storage(std::vector<Ts>&&...) -> Polymorphic_Storage<Ts...>;

}// sage::util

template <size_t I, typename... Ts>
struct std::tuple_element<I, sage::util::Polymorphic_Storage<Ts...>>
	: std::tuple_element<I, typename sage::util::Polymorphic_Storage<Ts...>::Base>
{};

template <typename... Ts>
struct std::tuple_size<sage::util::Polymorphic_Storage<Ts...>>
	: std::tuple_size<typename sage::util::Polymorphic_Storage<Ts...>::Base>
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

#ifdef SAGE_UTIL_TEST

#include "src/math.hpp"

namespace {

using namespace sage;

TEST_CASE ("Trim") {
	auto input = std::string{},
		 result = std::string{};
	auto to_trim = ' ';

	SUBCASE ("Empty") {
		input = ""s;
		result = ""s;
	}

	SUBCASE ("Trimable spaces only left") {
		input = "            Test 123!"s;
		result = "Test 123!";
	}

	SUBCASE ("Trimable spaces only right") {
		input = "Test 123!            "s;
		result = "Test 123!";
	}

	SUBCASE ("Trimable spaces both sides") {
		input = "                    Test 123!            "s;
		result = "Test 123!";
	}

	SUBCASE ("No trimable spaces") {
		input = "Test 123!"s;
		result = "Test 123!";
	}

	SUBCASE ("Trim other character") {
		input = "!!!!!!!!!!!Test 0!!!!!!!!!!!!!!!!!!!!!!!";
		result = "Test 0";
		to_trim = '!';
	}

	string::trim(input, to_trim);
	CAPTURE(std::quoted(input));
	CAPTURE(std::quoted(result));
	CHECK_EQ(result, input);
}

TEST_CASE ("Type") {
	CHECK_EQ(type::Any<int>,				false);
	CHECK_EQ(type::Any<int, int>,			true);
	CHECK_EQ(type::Any<int, float, int>,	true);
	CHECK_EQ(type::Any<int, double>,		false);

	CHECK_EQ(type::Not_In<int>,				true);
	CHECK_EQ(type::Not_In<int, int>,		false);
	CHECK_EQ(type::Not_In<int, float, int>,	false);
	CHECK_EQ(type::Not_In<int, double>,		true);

	CHECK_EQ(type::Unique<>,				true);
	CHECK_EQ(type::Unique<int>,				true);
	CHECK_EQ(type::Unique<int, float, int>,	false);
	CHECK_EQ(type::Unique<int, double>,		true);

	// TODO: Weird behaviour for Set<>::count<>()
	//CHECK_EQ(type::Set<>					::template count<>(), std::tuple{});
	CHECK_EQ(type::Set<>					::template count<int>(),						std::tuple{});
	CHECK_EQ(type::Set<int>					::template count<int>(),						std::make_tuple(1u));
	CHECK_EQ(type::Set<int, float>			::template count<float>(),						std::make_tuple(0u, 1u));
	CHECK_EQ(type::Set<int, float>			::template count<float, int, float>(),			std::make_tuple(1u, 2u));
	CHECK_EQ(type::Set<int, float>			::template count<float, int, double, float>(),	std::make_tuple(1u, 2u));

	CHECK_EQ(type::Set<>					::template contains<int>(), false);
	CHECK_EQ(type::Set<>					::template contains<>(),	false);
	CHECK_EQ(type::Set<int>					::template contains<int>(), true);
	CHECK_EQ(type::Set<int, float>			::template contains<int>(), true);
	CHECK_EQ(type::Set<std::string, float>	::template contains<int>(), false);
}

TEST_CASE ("Polymorphic_Array") {
	using Array = util::Polymorphic_Array<int, float, std::string>;

	SUBCASE ("Default Construction") {
		auto storage = Array{};
	}

	SUBCASE ("Initialized") {
		// FIXME: Add constness, see pragma in Polymorphic_Array
		auto storage = Array{1, 2.0f, "3"s};
		auto order_flags = std::bitset<3>();

		CAPTURE(storage);

		storage.apply([&] (const auto& x) {
				using T = std::decay_t<decltype(x)>;
				if constexpr (std::same_as<T, int>) {
					REQUIRE_MESSAGE(order_flags.none(), "Implementation does not keep template order");
					CHECK_EQ(x, 1);
					order_flags.flip(0);
				}
				else if constexpr (std::same_as<T, float>) {
					REQUIRE_MESSAGE((order_flags.count() == 1 and order_flags[0]), "Implementation does not keep template order");
					CHECK_EQ(x, 2.0);
					order_flags.flip(1);
				}
				else if constexpr (std::same_as<T, std::string>) {
					REQUIRE_MESSAGE((order_flags.count() == 2 and order_flags[0] and order_flags[1]), "Implementation does not keep template order");
					CHECK_EQ(x, "3");
					order_flags.flip(2);
				}
				else
					FAIL("Unexpected type in Polymorphic_Array");
			});

		CHECK(order_flags.all());
	}
}

TEST_CASE ("Polymorphic_Storage") {
	using Storage = util::Polymorphic_Storage<int, float, std::string>;

	SUBCASE ("Default Construction") {
		auto storage = Storage{};
	}

	SUBCASE ("Initialized") {
		// FIXME: Add constness, see pragma in Polymorphic_Array
		auto storage = Storage{
			{1,2,3,4,5},
			{1.f, 2.f, 3.f},
			{"1"s, "2"s}
		};

		CAPTURE(storage);

		storage.apply_group([] (const auto& vec) {
				using T = typename std::decay_t<decltype(vec)>::value_type;
				const auto capacity = vec.capacity(),
						   size = vec.size();
				CHECK_EQ(capacity, size);
				if constexpr (std::same_as<int, T>)
					CHECK_EQ(size, 5);
				else if constexpr (std::same_as<float, T>)
					CHECK_EQ(size, 3);
				else if constexpr (std::same_as<T, std::string>)
					CHECK_EQ(size, 2);
				else
					FAIL("Unexpected type in Polymorphic_Array");
			});
	}
}

TEST_CASE ("toogle_if") {
	auto b = true;

	for (auto i = 0; i < 1000; ++i) {
		const auto cond = random::toggle::any();
		const auto prev = b;

		b = toogle_if(b, cond);
		MESSAGE(b);
		if (cond)
			REQUIRE_NE(b, prev);
		else
			REQUIRE_EQ(b, prev);
	}
}

}
#endif

