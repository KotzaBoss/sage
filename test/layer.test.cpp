#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test/test.hpp"

#include "test/placeholder_layers.hpp"

using namespace sage;

TEST_CASE ("Layer") {
	using DL = Dump_Layer<input::Null, graphics::renderer::Null, layer::Null_User_State>;
	using OL = Other_Layer<input::Null, graphics::renderer::Null, layer::Null_User_State>;
	using LL = Last_Layer<input::Null, graphics::renderer::Null, layer::Null_User_State>;
	using Layers = layer::Storage<DL, OL, LL>;

	auto layers = Layers{
		{
			DL{1},
			DL{2},
			DL{3},
			DL{4},
			DL{5},
		},
		{
			OL{6},
			OL{7},
			OL{8},
		},
		{
			LL{9},
			LL{10},
			LL{11},
			LL{12},
		},
	};

	SUBCASE ("Layer access sequence") {
		auto ids = std::vector<size_t>{};
		ids.reserve(layers.size());

		layers.apply([&] (const auto& layer) {
				ids.push_back(layer.id);
			});

		INFO(fmt::format("{}", ids));
		REQUIRE(rg::is_sorted(ids));
	}

	const auto start = std::chrono::steady_clock::now();
	for (auto tick = sage::Tick{}; tick.current_time_point() < start + 1s; ) {
		layers.update(tick(), input::null, layer::null_user_state);
	}
}

