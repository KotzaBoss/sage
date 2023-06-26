#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "std.hpp"

#include "placeholder_layers.hpp"
#include "layer.hpp"

TEST_CASE ("Layer") {
	// Layers are added as follows (example):
	//
	// Layers<A, B, C> { ... }
	// A1, A2, ... AN,
	// B1, B2, ... BN,
	// C1, C2, ... CN
	//
	// To confirm that make sure the layer ids are such that the above is satisfied.
	// Order of Layers in the constructor doesnt matter, but the ids per object must be
	// correct.
	auto layers = sage::layer::Layers<Dump_Layer, Other_Layer, Last_Layer>{
		Last_Layer{9},
		Dump_Layer{1},
		Dump_Layer{2},
		Last_Layer{10},
		Last_Layer{11},
		Other_Layer{6},
		Dump_Layer{3},
		Dump_Layer{4},
		Other_Layer{7},
		Other_Layer{8},
		Last_Layer{12},
		Dump_Layer{5},
	};

	MESSAGE(layers);

	SUBCASE ("Layer access sequence") {
		auto ids = std::vector<size_t>{};
		ids.reserve(layers.size());

		layers.const_apply([&] (const auto& layer) {
				ids.push_back(layer.id);
			});

		INFO(fmt::format("{}", ids));
		REQUIRE(rg::is_sorted(ids));
	}

	layers.setup();

	for (auto i = 0; i < 5; ++i) {
		MESSAGE("Update ", i, " =========================");
		layers.update();
	}

	layers.teardown();
}

