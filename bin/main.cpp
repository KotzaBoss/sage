#include "src/sage.hpp"

#include "bin/layer_2d.hpp"

using namespace sage;

struct App_Rendering {
	using Renderer = oslinux::Renderer_2D;
	using Drawings = std::tuple<oslinux::Texture2D, glm::vec4>;
};

struct App_Layer_Spec {
	using Layer = Layer_2D;
	using Rendering = App_Rendering;
	using Input = oslinux::Input;
};

using The_App = sage::App<
	oslinux::Window,
	oslinux::Input,
	App_Rendering,
	App_Layer_Spec
>;

auto main() -> int {
	static auto stop_source = std::stop_source{};
	signal(SIGINT, [] (int) { stop_source.request_stop(); });

	//

	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	auto app = The_App {
			std::move(win),
			std::move(input)
		};

	return app.run(stop_source.get_token());
}
