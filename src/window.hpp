#pragma once

#include "std.hpp"

#include "math.hpp"

namespace sage::inline window {

struct Window {
	struct Properties {
		std::string title = "SAGE Window"s;
		Size size = { .width=1280, .height=720 };
	public:
		friend auto operator<< (std::ostream& o, const Properties& p) -> std::ostream& {
			return o << "Properties: title=" << std::quoted(p.title) << " size=" << p.size;
		}
	};

	using Fn = std::function<void()>;

protected:
	Properties properties;

private:
	Fn _setup, _update, _teardown;

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
	inline auto setup()	-> void { std::invoke(_setup); }
	inline auto update()	-> void { std::invoke(_update); }
	inline auto teardown()	-> void { std::invoke(_teardown); }

public:
	friend auto operator<< (std::ostream& o, const Window& w) -> std::ostream& {
		return o << "Window: " << w.properties;
	}
};

}// sage::window
