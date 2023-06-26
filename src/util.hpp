#pragma once

#include "std.hpp"

#include "repr.hpp"

namespace sage::inline util {

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

	// FIXME: Hate this, i couldnt get operator= to compile correctly, it keeps thinking that
	// monitor = object;
	// is trying to assign a Monitor<Object> instead of an Object.
	template <typename Obj>
		// Could not really figure out how to constrain it without this monstrosity.
		// std::assignable_from whines about the LHS not being a lvalue.
		requires requires (Obj o, Object dest) { dest = o; }
	auto assign(Obj&& o) const -> const Monitor& {
		LOCK_GUARD(m);
		obj = std::forward<Obj>(o);
		return *this;
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


