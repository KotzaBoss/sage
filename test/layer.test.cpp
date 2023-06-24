#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "std.hpp"

#include "layer.hpp"

using namespace sage;

struct Dump_Layer {
	size_t id;

	Dump_Layer(const size_t _id)
		: id{_id}
	{}

	auto setup() -> void { MESSAGE("DUMP Setting up ", id); }
	auto update() -> void { MESSAGE("DUMP Updating ", id); }
	auto teardown() -> void { MESSAGE("DUMP Tearing down ", id); }
	auto event_callback(const auto& event) -> void { MESSAGE("DUMP ", id, "got Event ", event); }
};

struct Other_Layer {
	size_t id;

	Other_Layer(const size_t _id)
		: id{_id}
	{}

	auto setup() -> void { MESSAGE("OTHER Setting up ", id); }
	auto update() -> void { MESSAGE("OTHER Updating ", id); }
	auto teardown() -> void { MESSAGE("OTHER Tearing down ", id); }
	auto event_callback(const auto& event) -> void { MESSAGE("OTHER Layer \"", id, "\" got Event ", event); }
};

struct Last_Layer {
	size_t id;

	Last_Layer(const size_t _id)
		: id{_id}
	{}

	auto setup() -> void { MESSAGE("LAST Setting up ", id); }
	auto update() -> void { MESSAGE("LAST Updating ", id); }
	auto teardown() -> void { MESSAGE("LAST Tearing down ", id); }
	auto event_callback(const auto& event) -> void { MESSAGE("LAST Layer \"", id, "\" got Event ", event); }
};

TEST_CASE ("Layer") {

	auto layers = sage::layer::Layers<Dump_Layer, Other_Layer, Last_Layer>{
		Last_Layer{0},
		Dump_Layer{1},
		Dump_Layer{2},
		Last_Layer{1},
		Last_Layer{2},
		Other_Layer{0},
		Dump_Layer{3},
		Dump_Layer{4},
		Other_Layer{1},
		Other_Layer{2},
		Last_Layer{3},
		Dump_Layer{5},
	};

	layers.setup();

	for (auto i = 0; i < 5; ++i) {
		MESSAGE("Update ", i, " =========================");
		layers.update();
	}

	layers.teardown();
}

