#include "src/repr.hpp"
#include "src/event.hpp"

#include "imgui.h"

struct Non_State {};

struct Dump_Layer {
	template <typename _Input, typename _Rendering, typename _User_State>
	struct Spec {
		using Layer = Dump_Layer;
		using Rendering = _Rendering;
		using Input = _Input;
		using User_State = _User_State;
	};

public:
	size_t id;

	Dump_Layer(const size_t _id)
		: id{_id}
	{
		MESSAGE("DUMP Setting up ", id);
	}

	~Dump_Layer() { MESSAGE("DUMP Tearing down ", id); }

	auto update(const std::chrono::milliseconds delta, auto&, auto&) -> void { MESSAGE("DUMP Updating {}", id, delta); }
	auto render(auto&, auto&) {}
	auto imgui_prepare() -> void {
		// Create a window called "My First Tool", with a menu bar.
		static auto my_tool_active = true;
		ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
		    if (ImGui::BeginMenu("File"))
		    {
		        if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
		        if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
		        if (ImGui::MenuItem("Close", "Ctrl+W"))  { my_tool_active = false; }
		        ImGui::EndMenu();
		    }
		    ImGui::EndMenuBar();
		}

		// Edit a color stored as 4 floats
		float my_color[] = { 1.f, 2.f, 3.f, 4.f };
		ImGui::ColorEdit4("Color", my_color);

		// Generate samples and plot them
		float samples[100];
		for (int n = 0; n < 100; n++)
		    samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
		ImGui::PlotLines("Samples", samples, 100);

		// Display contents in a scrolling region
		ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
		ImGui::BeginChild("Scrolling");
		for (int n = 0; n < 50; n++)
		    ImGui::Text("%04d: Some text", n);
		ImGui::EndChild();
		ImGui::End();
	}
	auto event_callback(const sage::Event& event, auto&) -> void { MESSAGE("DUMP ", id, "got Event ", event); }

	REPR_DECL(Dump_Layer);
};

template <>
FMT_FORMATTER(Dump_Layer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Dump_Layer) {
		return fmt::format_to(ctx.out(), "Dump_Layer: {}", obj.id);
	}
};

REPR_DEF_FMT(Dump_Layer);

struct Other_Layer {
	template <typename _Input, typename _Rendering, typename _User_State>
	struct Spec {
		using Layer = Other_Layer;
		using Rendering = _Rendering;
		using Input = _Input;
		using User_State = _User_State;
	};

public:
	size_t id;

	Other_Layer(const size_t _id)
		: id{_id}
	{
		MESSAGE("OTHER Setting up ", id);
	}

	~Other_Layer() { MESSAGE("OTHER Tearing down ", id); }

	auto update(const std::chrono::milliseconds delta, auto&, auto&) -> void { MESSAGE("OTHER Updating {}", id, delta); }
	auto render(auto&, auto&) {}
	auto imgui_prepare() -> void {
		ImGui::Text("Hello, world %d", 123);
		if (ImGui::Button("Save"))
		    SAGE_LOG_INFO("Saving");
		std::string buf{10};
		ImGui::InputText("string", buf.data(), buf.size());
		float f;
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	}
	auto event_callback(const sage::Event& event, auto&) -> void { MESSAGE("OTHER Layer \"", id, "\" got Event ", event); }

	REPR_DECL(Other_Layer);
};

template <>
FMT_FORMATTER(Other_Layer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Other_Layer) {
		return fmt::format_to(ctx.out(), "Other_Layer: {}", obj.id);
	}
};

REPR_DEF_FMT(Other_Layer);

struct Last_Layer {
	template <typename _Input, typename _Rendering, typename _User_State>
	struct Spec {
		using Layer = Last_Layer;
		using Rendering = _Rendering;
		using Input = _Input;
		using User_State = _User_State;
	};

public:
	size_t id;

	Last_Layer(const size_t _id)
		: id{_id}
	{
		MESSAGE("DUMP Setting up ", id);
	}

	~Last_Layer() { MESSAGE("DUMP Tearing down ", id); }

	auto update(const std::chrono::milliseconds delta, auto&, auto&) -> void { MESSAGE("LAST Updating {}", id, delta); }
	auto render(auto&, auto&) {}
	auto imgui_prepare() -> void {
		ImGui::Text(fmt::format("Brrrrrrrrrrrrrr {}", id).c_str());
	}
	auto event_callback(const sage::Event& event, auto&) -> void { MESSAGE("DUMP ", id, "got Event ", event); }

	REPR_DECL(Last_Layer);
};

template <>
FMT_FORMATTER(Last_Layer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Last_Layer) {
		return fmt::format_to(ctx.out(), "Last_Layer: {}", obj.id);
	}
};

REPR_DEF_FMT(Last_Layer);
