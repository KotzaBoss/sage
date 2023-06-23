#pragma once

#include "std.hpp"

#include "event.hpp"
#include "math.hpp"
#include "repr.hpp"

namespace sage::inline window {

struct Window {
	struct Properties {
		using Size = sage::Size<size_t>;

		std::string title = "SAGE Window"s;
		Size size = { .width=1280, .height=720 };
	public:
		REPR_DECL(Window::Properties);
	};

	using Fn = std::function<void()>;

protected:
	Properties _properties;

private:
	Fn _setup, _update, _teardown;
	Event::Callback _event_callback;

public:
	struct Args {
		Properties&& properties;
		Fn&& setup, update, teardown;
	};
	Window(Args&& args);

public:
	// Implement all per platfrom
	static auto make(Properties&& props) -> Window&;

public:
	auto setup		(Event::Callback&& cb)	-> void;
	auto update		()						-> void;
	auto teardown	()						-> void;

	auto event_callback(const Event& e) -> void;

	inline auto properties() const -> const Properties& { return _properties; }

public:
	REPR_DECL(Window);
};

}// sage::window
