#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "sage.hpp"
#include "platform/linux/window.hpp"
#include "platform/linux/input.hpp"

#include "layer_imgui.hpp"
#include "placeholder_layers.hpp"

using namespace sage;

struct Instrumented_ImGui : layer::ImGui {
	using layer::ImGui::ImGui;

	auto event_callback(const Event& e) -> void {
		MESSAGE(e);
	}

	REPR_DECL(Instrumented_ImGui);
};

template<>
FMT_FORMATTER(Instrumented_ImGui) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Instrumented_ImGui) {
		return fmt::format_to(ctx.out(), "Instrumented_ImGui");
	}
};

REPR_DEF_FMT(Instrumented_ImGui);


TEST_CASE ("App") {
	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	auto imgui = Instrumented_ImGui{win.native_handle()};
	auto app = sage::App<oslinux::Window, oslinux::Input, Instrumented_ImGui, Other_Layer, Last_Layer>(
			std::move(win),
			std::move(input),
			{
				std::move(imgui),
				Last_Layer{3},
				Other_Layer{1},
				Other_Layer{2},
				Last_Layer{4},
				}
		);
	MESSAGE(app);
	app.start();
	std::this_thread::sleep_for(3s);
	app.stop();
}
