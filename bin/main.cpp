#include "sage.hpp"

#include "platform/linux/window.hpp"
#include "layer_2d.hpp"

using namespace sage;

auto main() -> int {
	static auto stop_source = std::stop_source{};
	signal(SIGINT, [] (int) { stop_source.request_stop(); });

	//

	struct App_Rendering {
		using Renderer = oslinux::Renderer_2D;
		using Drawings = std::tuple<oslinux::Texture2D, glm::vec4>;
	};

	struct App_Layer_Spec {
		using Layer = Layer_2D;
		using Rendering = App_Rendering;
	};

	using App = sage::App<
			oslinux::Window,
			App_Rendering,
			App_Layer_Spec
		>;

	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	auto renderer = oslinux::Renderer_2D{};
	auto layer_2d = Layer_2D{input};
	auto app = App {
			std::move(win),
			std::move(layer_2d)
		};

	return app.run(stop_source.get_token());
}
