#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "sage.hpp"
#include "platform/linux/window.hpp"

using namespace sage;

struct Placeholder_Layer : sage::Layer {
	std::string name;

	Placeholder_Layer(std::string&& n)
		: sage::Layer{{
			.setup = [this, n = std::move(n)] {
					name = std::move(n);
					MESSAGE("Setting up ", name);
			},
			.update = [this] {
					MESSAGE("Updating ", name);
			},
			.teardown = [this] {
					MESSAGE("Tearing down ", name);
			},
			.event_callback = [this] (const auto& event) {
					MESSAGE("Layer '", name, "' got Event ", event);
			}
		}}
	{}
};

TEST_CASE ("App") {
	auto pl1 = Placeholder_Layer{"PL1"s},
		 pl2 = Placeholder_Layer{"PL2"s};
	auto app = sage::App<oslinux::Window>(
			window::Properties{},
			Layers{pl1, pl2}
		);
	MESSAGE(app);
	app.start();
	std::this_thread::sleep_for(500ms);
	app.stop();
}
