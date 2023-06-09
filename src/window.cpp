#include "window.hpp"
#include "log.hpp"

namespace sage::window {

Window::Window(Args&& args)
	: properties{std::move(args.properties)}
	, _setup{std::move(args.setup)}
	, _update{std::move(args.update)}
	, _teardown{std::move(args.teardown)}
{}



}// sage::window
