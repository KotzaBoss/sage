#pragma once

#include "std.hpp"

#include "event.hpp"
#include "ref_wrapper.hpp"

namespace sage::inline layer {

struct Layer {
	using Fn = std::function<void()>;

private:
	Fn _setup, _update, _teardown;
	Event::Callback _event_callback;

public:
	struct Args {
		Fn&& setup, update, teardown;
		Event::Callback&& event_callback;
	};
	Layer(Args&& args);

public:
	auto setup() -> void;
	auto update() -> void;
	auto teardown() -> void;
	auto event_callback (const Event&) -> void;
};

struct Layers {
	using Stack = std::vector<Ref_Wrapper<Layer>>;

private:
	Stack stack;

public:
	Layers(auto&... layers)
		: stack{layers...}
	{}

public:
	auto setup() -> void;
	auto update() -> void;
	auto teardown() -> void;
	auto event_callback(const Event& e) -> void;
};

}// sage::layer
