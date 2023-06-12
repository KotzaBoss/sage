#include "event.hpp"

using namespace sage;

auto main() -> int {
	std::cerr << Event::make_window_closed() << '\n';
	for (auto i = 0; i < 10; ++i)
		std::cerr << Event::make_window_resized(Size<int>{std::rand() % 4000, std::rand() % 4000}) << '\n';
}
