#pragma once

#include "std.hpp"

#include "event.hpp"
#include "math.hpp"

namespace sage::inline window {

struct Window {
	struct Properties {
		std::string title = "SAGE Window"s;
		Size<size_t> size = { .width=1280, .height=720 };
	public:
		friend auto operator<< (std::ostream& o, const Properties& p) -> std::ostream& {
			return o << "Properties: title=" << std::quoted(p.title) << " size=" << p.size;
		}
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
	friend auto operator<< (std::ostream& o, const Window& w) -> std::ostream& {
		return o << "Window: " << w.properties;
	}
};

}// sage::window
