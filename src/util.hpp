#pragma once

#include "std.hpp"

#ifndef __cpp_lib_to_underlying
namespace std {
constexpr auto to_underlying(auto e) -> auto {
	return static_cast<std::underlying_type_t<decltype(e)>>(e);
}
}// std
#endif

#include "repr.hpp"
#include "log.hpp"

namespace sage::inline util {

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
consteval auto bits() -> size_t {
	return sizeof(I) * 8;
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

	auto front() -> std::tuple_element_t<0, Storage>::reference {
		return std::get<0>(storage).front();
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


