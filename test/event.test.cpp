#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "event.hpp"

using namespace sage;

TEST_CASE ("Event") {
	MESSAGE(Event::make_window_closed());
	for (auto i = 0; i < 10; ++i)
		MESSAGE(Event::make_window_resized(Size<int>{std::rand() % 4000, std::rand() % 4000}));
}
