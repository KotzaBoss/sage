#pragma once

#include "src/std.hpp"

#include "src/graphics.hpp"
#include "src/event.hpp"
#include "src/input.hpp"
#include "src/camera.hpp"
#include "src/util.hpp"

#include "src/repr.hpp"

namespace sage::layer {

template <typename Layer>
concept Concept =
		requires { typename Layer::Input; } and input::Concept<typename Layer::Input>
	and requires { typename Layer::Renderer; } and graphics::renderer::Concept_2D<typename Layer::Renderer>
	and requires { typename Layer::User_State; }

	and requires (Layer l, const std::chrono::milliseconds delta, Layer::Input& input, camera::Controller<typename Layer::Input>& cam, Layer::User_State& user_state) {
		{ l.update(delta, input, cam, user_state) } -> std::same_as<void>;
	}
	and requires (Layer l, Layer::Renderer& renderer, Layer::User_State& user_state) {
		{ l.render(renderer, user_state) } -> std::same_as<void>;
	}
	and requires (Layer l, const Event& event, camera::Controller<typename Layer::Input>& cam, Layer::User_State& user_state) {
		{ l.event_callback(event, cam, user_state) } -> std::same_as<void>;
	}
	and requires (Layer l, camera::Controller<typename Layer::Input>& cam_contr, Layer::Renderer::Frame_Buffer& frame_buffer, Layer::User_State& user_state) {
		{ l.imgui_prepare(cam_contr, frame_buffer, user_state) } -> std::same_as<void>;	// Must be called in layer::ImGui::new_frame()
	}
	;

struct Null_User_State {};
inline auto null_user_state = Null_User_State{};

template<typename... Ls>
concept Are_Coherent =
		(layer::Concept<Ls> and ...)
	and	type::All<typename Ls::Input...>
	and type::All<typename Ls::Renderer...>
	and type::All<typename Ls::User_State...>
	;

template <layer::Concept... Ls>
	requires Are_Coherent<Ls...>
struct Array : util::Polymorphic_Array<Ls...> {
	using Base = util::Polymorphic_Array<Ls...>;
	using Input = type::Front<typename Ls::Input...>;
	using Renderer = type::Front<typename Ls::Renderer...>;
	using Frame_buffer = Renderer::Frame_Buffer;
	using User_State = type::Front<typename Ls::User_State...>;
	using Camera_Controller = camera::Controller<Input>;

public:
	Array(type::Any<Ls...> auto&&... ls)
		: Base{std::move(ls)...}
	{}

public:
	auto update(const std::chrono::milliseconds delta, Input& input, camera::Controller<Input>& cam, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.update(delta, input, cam, user_state);
			});
	}

	auto render(Renderer& renderer, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.render(renderer, user_state);
			});
	}

	auto event_callback(const Event& e, camera::Controller<Input>& cam, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.event_callback(e, cam, user_state);
			});
	}

	auto imgui_prepare(camera::Controller<Input>& cc, Renderer::Frame_Buffer& fb, User_State& us) -> void {
		Base::apply([&] (auto& layer) {
				layer.imgui_prepare(cc, fb, us);
			});
	}


public:
	friend REPR_DEF_FMT(Array<Ls...>)
	friend FMT_FORMATTER(Array<Ls...>);

};

template <layer::Concept... Ls>
	requires Are_Coherent<Ls...>
struct Storage : util::Polymorphic_Storage<Ls...> {
	using Base = util::Polymorphic_Storage<Ls...>;
	using Input = type::Front<typename Ls::Input...>;
	using Renderer = type::Front<typename Ls::Renderer...>;
	using Frame_buffer = Renderer::Frame_Buffer;
	using User_State = type::Front<typename Ls::User_State...>;
	using Camera_Controller = camera::Controller<Input>;

public:
	Storage(typename Base::Vector<Ls>&&... layers)
		: Base{std::move(layers)...}
	{}

public:
	auto update(const std::chrono::milliseconds delta, Input& input, camera::Controller<Input>& cam, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.update(delta, input, cam, user_state);
			});
	}

	auto render(Renderer& renderer, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.render(renderer, user_state);
			});
	}

	auto event_callback(const Event& e, camera::Controller<Input>& cam, User_State& user_state) -> void {
		Base::apply([&] (auto& layer) {
				layer.event_callback(e, cam, user_state);
			});
	}

	auto imgui_prepare(camera::Controller<Input>& cc, Renderer::Frame_Buffer& fb, User_State& us) -> void {
		Base::apply([&] (auto& layer) {
				layer.imgui_prepare(cc, fb, us);
			});
	}



public:
	friend REPR_DEF_FMT(Storage<Ls...>)
	friend FMT_FORMATTER(Storage<Ls...>);
};

}// sage::layer

template <typename... Ls>
FMT_FORMATTER(sage::layer::Array<Ls...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::layer::Array<Ls...>) {
		return fmt::format_to(ctx.out(), "layer::Array: {};", static_cast<const std::decay_t<decltype(obj)>::Base&>(obj));
	}
};

template <typename... Ls>
FMT_FORMATTER(sage::layer::Storage<Ls...>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::layer::Storage<Ls...>) {
		return fmt::format_to(ctx.out(), "layer::Storage: {};", static_cast<const std::decay_t<decltype(obj)>::Base&>(obj));
	}
};

#ifdef SAGE_TEST_LAYER
namespace {

#include "test/placeholder_layers.hpp"

using namespace sage;

TEST_CASE ("Layer") {

	using DL = Dump_Layer<input::Null, graphics::renderer::Null, layer::Null_User_State>;
	using OL = Other_Layer<input::Null, graphics::renderer::Null, layer::Null_User_State>;
	using LL = Last_Layer<input::Null, graphics::renderer::Null, layer::Null_User_State>;
	using Layers = layer::Storage<DL, OL, LL>;

	auto layers = Layers{
		{
			DL{1},
			DL{2},
			DL{3},
			DL{4},
			DL{5},
		},
		{
			OL{6},
			OL{7},
			OL{8},
		},
		{
			LL{9},
			LL{10},
			LL{11},
			LL{12},
		},
	};

	SUBCASE ("Layer access sequence") {
		auto ids = std::vector<size_t>{};
		ids.reserve(layers.size());

		layers.apply([&] (const auto& layer) {
				ids.push_back(layer.id);
			});

		INFO(fmt::format("{}", ids));
		REQUIRE(rg::is_sorted(ids));
	}

	const auto start = std::chrono::steady_clock::now();
	auto& cam = camera::Controller<input::Null>::null();
	for (auto tick = sage::Tick{}; tick.current_time_point() < start + 1s; ) {
		layers.update(tick(), input::null, cam, layer::null_user_state);
		layers.render(graphics::renderer::null, layer::null_user_state);
		layers.event_callback(Event{}, cam, layer::null_user_state);
	}
}


}
#endif
