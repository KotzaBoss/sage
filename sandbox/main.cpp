#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "sage.hpp"
#include "platform/linux/window.hpp"
#include "platform/linux/input.hpp"

#include "layer_imgui.hpp"

using namespace sage;

struct Some_Layer {
	std::string text;

	auto setup() {}
	auto update() {}
	auto imgui_prepare() -> void {
		ImGui::Text(text.c_str());
	}
	auto teardown() {}
	auto event_callback(const Event&) {}

	REPR_DECL(Some_Layer);
};

template <>
FMT_FORMATTER(Some_Layer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Some_Layer) {
		return fmt::format_to(ctx.out(), "Some_Layer {}", obj.text);
	}
};

REPR_DEF_FMT(Some_Layer);

TEST_CASE ("App") {
	auto win = oslinux::Window{window::Properties{}};
	auto input = oslinux::Input{win.native_handle()};
	auto app = sage::App<oslinux::Window, oslinux::Input, Some_Layer>(
			std::move(win),
			std::move(input),
			Some_Layer{"blah"},
			Some_Layer{"bleh"}
		);
	SAGE_LOG_INFO(app);

	app.start();

	static auto exit = false;
	signal(SIGINT, [] (int) { exit = true; });
	while (not exit)
		;

	app.stop();
}
