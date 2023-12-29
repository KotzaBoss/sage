#include "src/sage.hpp"

#include "bin/sandbox.hpp"
#include "bin/editor.hpp"

using namespace sage;

using The_App = sage::App<
	oslinux::Window,
	oslinux::Input,
	oslinux::Renderer_2D,
	Game_State,
	Layer_2D,
	Rocket_Flame,
	Editor
>;

auto main() -> int {
	static auto stop_source = std::stop_source{};
	signal(SIGINT, [] (int) { stop_source.request_stop(); });

	//

	auto app = The_App{};

	return app.run(stop_source.get_token());
}
