#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "std.hpp"

#include "layer.hpp"

using namespace sage;

struct Dump_Layer : Layer {
	size_t id;

	Dump_Layer(const size_t _id)
		: sage::Layer{{
			.setup = [this, _id] {
					id = _id;
					MESSAGE("Setting up ", id);
			},
			.update = [this] {
					MESSAGE("Updating ", id);
			},
			.teardown = [this] {
					MESSAGE("Tearing down ", id);
			},
			.event_callback = [this] (const auto& event) {
					MESSAGE("Layer ", id, "got Event ", event);
			}
		}}
	{}
};

TEST_CASE ("Layer") {

	auto dl1 = Dump_Layer{1},
		 dl2 = Dump_Layer{2};

	auto layers = sage::Layers{dl1, dl2};

	layers.setup();

	for (auto i = 0; i < 10; ++i)
		layers.update();

	layers.teardown();
}

