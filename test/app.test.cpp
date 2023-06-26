#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "sage.hpp"
#include "platform/linux/window.hpp"

#include "placeholder_layers.hpp"

using namespace sage;

TEST_CASE ("App") {
	auto app = sage::App<oslinux::Window, Dump_Layer, Other_Layer, Last_Layer>(
			window::Properties{},
			{
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
				}
		);
	MESSAGE(app);
	app.start();
	std::this_thread::sleep_for(2s);
	app.stop();
}
