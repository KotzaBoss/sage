#include "src/repr.hpp"
#include "src/event.hpp"

#include "imgui.h"

template <typename _Input, typename _Renderer, typename _User_State>
struct Dump_Layer {
	using Renderer = _Renderer;
	using Input = _Input;
	using User_State = _User_State;

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
	auto imgui_prepare(auto&) -> void {
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
};

template <typename _Input, typename _Renderer, typename _User_State>
struct Other_Layer {
	using Renderer = _Renderer;
	using Input = _Input;
	using User_State = _User_State;

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
	auto imgui_prepare(auto&) -> void {
		ImGui::Text("Hello, world %d", 123);
		if (ImGui::Button("Save"))
		    SAGE_LOG_INFO("Saving");
		std::string buf{10};
		ImGui::InputText("string", buf.data(), buf.size());
		float f;
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	}
	auto event_callback(const sage::Event& event, auto&) -> void { MESSAGE("OTHER Layer \"", id, "\" got Event ", event); }
};

template <typename _Input, typename _Renderer, typename _User_State>
struct Last_Layer {
	using Renderer = _Renderer;
	using Input = _Input;
	using User_State = _User_State;

public:
	size_t id;

	Last_Layer(const size_t _id)
		: id{_id}
	{
		MESSAGE("LAST Setting up ", id);
	}

	~Last_Layer() { MESSAGE("LAST Tearing down ", id); }

	auto update(const std::chrono::milliseconds delta, auto&, auto&) -> void { MESSAGE("LAST Updating {}", id, delta); }
	auto render(auto&, auto&) {}
	auto imgui_prepare(auto&) -> void {
		ImGui::Text(fmt::format("Brrrrrrrrrrrrrr {}", id).c_str());
	}
	auto event_callback(const sage::Event& event, auto&) -> void { MESSAGE("LAST ", id, "got Event ", event); }
};

