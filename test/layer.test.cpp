#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test/test.hpp"

#include "test/placeholder_layers.hpp"

using namespace sage;

TEST_CASE ("Layer") {
	using Null_Rendering = graphics::renderer::Null_Rendering;
	using Layers = layer::Storage<
			Dump_Layer::Spec<input::Null, Null_Rendering, layer::Null_User_State>,
			Other_Layer::Spec<input::Null, Null_Rendering, layer::Null_User_State>,
			Last_Layer::Spec<input::Null, Null_Rendering, layer::Null_User_State>
		>;


	auto layers = Layers{
		{
			Dump_Layer{1},
			Dump_Layer{2},
			Dump_Layer{3},
			Dump_Layer{4},
			Dump_Layer{5},
		},
		{
			Other_Layer{6},
			Other_Layer{7},
			Other_Layer{8},
		},
		{
			Last_Layer{9},
			Last_Layer{10},
			Last_Layer{11},
			Last_Layer{12},
		},
	};

	MESSAGE(layers);

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

