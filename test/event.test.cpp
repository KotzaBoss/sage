#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test/test.hpp"

using namespace sage;

TEST_CASE ("Event") {
	MESSAGE(Event::make_window_closed());
	for (auto i = 0; i < 10; ++i)
		MESSAGE(Event::make_window_resized(Size<size_t>{(size_t)std::rand() % 4000, (size_t)std::rand() % 4000}));
}
