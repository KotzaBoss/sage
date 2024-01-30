#pragma once

#include "src/sage.hpp"

#include "src/platform/linux/sage.hpp"

using namespace sage;

struct Level {
private:
	oslinux::Renderer_2D::Texture atlas = {"asset/texture/kenney_rpg-base/Spritesheet/RPGpack_sheet_2X.png"};

	static constexpr auto cell_size = glm::vec2{ 128.f, 128.f };
	oslinux::Renderer_2D::Sub_Texture
		// Grass
		  grass_on_dirt_top_left	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 0, 12 }, .sprite_size = { 1, 1 } })
		, grass_on_dirt_top			= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 1, 12 }, .sprite_size = { 1, 1 } })
		, grass_on_dirt_top_right	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 2, 12 }, .sprite_size = { 1, 1 } })
		, grass_on_dirt_mid_left	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 0, 11 }, .sprite_size = { 1, 1 } })
		, grass_on_dirt_mid			= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 1, 11 }, .sprite_size = { 1, 1 } })
		, grass_on_dirt_mid_right	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 2, 11 }, .sprite_size = { 1, 1 } })
		, grass_on_dirt_bot_left	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 0, 10 }, .sprite_size = { 1, 1 } })
		, grass_on_dirt_bot			= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 1, 10 }, .sprite_size = { 1, 1 } })
		, grass_on_dirt_bot_right	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 2, 10 }, .sprite_size = { 1, 1 } })

		// Water
		, water	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 11, 11 }, .sprite_size = { 1, 1 } })

		// Dirt
		, dirt	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 6, 11 }, .sprite_size = { 1, 1 } })

		// Trees
		, tree_big		= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 2, 1 }, .sprite_size = { 1, 2 } })
		, tree_small	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 3, 1 }, .sprite_size = { 1, 2 } })

		// Building
		, building_bot_left		= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 1, 9 }, .sprite_size = { 1, 1 } })
		, building_bot_mid		= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 0, 9 }, .sprite_size = { 1, 1 } })
		, building_bot_right	= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 3, 9 }, .sprite_size = { 1, 1 } })
		, building_wall			= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 4, 9 }, .sprite_size = { 3, 1 } })
		, building_right		= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 3, 9 }, .sprite_size = { 1, 1 } })
		, building_door			= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 14, 2 }, .sprite_size = { 1, 1 } })
		, building_window		= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 10, 2 }, .sprite_size = { 1, 1 } })
		, building_roof			= oslinux::Renderer_2D::Sub_Texture(atlas, { .cell_size = cell_size, .offset = { 2, 4 }, .sprite_size = { 2, 3 } })
		;

	static constexpr auto rank = 10ul;
	std::array<std::array<ECS::Entity, rank>, rank> map;
	static constexpr auto color_water = glm::vec4{0.f, 0.f, 1.f, 1.f};
	static constexpr auto color_dirt = glm::vec4{1.f, 1.f, 0.f, 1.f};

private:
	ECS::Entity square;

public:
	Level(ECS& ecs) {
		for (auto& e : map[0]) {
			e = *ecs.create();
			e.set(component::Name{"Tile of Dirt"}, component::Sprite{color_dirt});
		}

		for (auto& row : map | vw::drop(1))
			for (auto& e : row) {
				e = *ecs.create();
				e.set(component::Name{"Tile of Water"}, component::Sprite{color_water});
			}
	}

public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input) {
	}

	auto render(oslinux::Renderer_2D& renderer) {
		using Simple_Args = oslinux::Renderer_2D::Simple_Args;

		constexpr auto size = glm::vec2{ 1.f, 1.f };

		SAGE_ASSERT(rg::all_of(map, [len = map.front().size()] (const auto& str) { return str.size() == len; }));

		for (const auto& [x, row] : map | vw::enumerate)
			for (const auto& [y, tile] : row | vw::enumerate)
				renderer.draw(std::get<0>(*tile.components<component::Sprite>())->color,
						Simple_Args{ .position={x, map.size() - y, 0.f}, .size={1, 1} }
					);

		if (square.is_valid()) {
			auto comps = square.components();
			SAGE_ASSERT(comps.has_value());

			auto& sprite = std::get<std::optional<component::Sprite>&>(*comps);
			auto& position = std::get<std::optional<component::Position>&>(*comps);
			SAGE_ASSERT(sprite.has_value());
			SAGE_ASSERT(position.has_value());

			renderer.draw(sprite->color, Simple_Args{ .position=position->position, .size={1, 1} });
		}
	}

	auto imgui_prepare(camera::Controller<oslinux::Input>& cam, ECS& ecs) {
		ImGui::Begin("Level");

		// TODO: Fix orientation of map
		auto flat_idx = 0ul;
		for (const auto& [x, row] : map | vw::enumerate) {
			for (const auto& [y, tile] : row | vw::enumerate) {
				auto comps = tile.components<component::Name, component::Sprite>();
				SAGE_ASSERT(comps.has_value());

				auto& [name, sprite] = *comps;
				SAGE_ASSERT(name.has_value());
				SAGE_ASSERT(sprite.has_value());

				if (y > 0)
					ImGui::SameLine();

				ImGui::PushID(flat_idx++);
				ImGui::PushStyleColor(ImGuiCol_Button,			ImVec4{sprite->color.r, sprite->color.g, sprite->color.b, sprite->color.a});
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered,	ImVec4{sprite->color.r, sprite->color.g, sprite->color.b, sprite->color.a});
				ImGui::PushStyleColor(ImGuiCol_ButtonActive,	ImVec4{sprite->color.r, sprite->color.g, sprite->color.b, sprite->color.a});

				if (ImGui::Button(" ")) {
					if (name->name.contains("Water")) {
						name = component::Name{"Tile of Dirt"s};
						sprite = component::Sprite{color_dirt};
					}
					else {
						name = component::Name{"Tile of Water"s};
						sprite = component::Sprite{color_water};
					}
				}

				ImGui::PopStyleColor(3);
				ImGui::PopID();
			}
		}


		ImGui::Separator();

		const auto square_is_valid = square.is_valid();

		{
			auto* color = std::invoke([&] {
					if (square_is_valid) {
						auto comps = square.components<component::Sprite>();
						SAGE_ASSERT(comps.has_value());
						auto& sprite = std::get<std::optional<component::Sprite>&>(*comps);
						SAGE_ASSERT(sprite.has_value());
						return glm::value_ptr(sprite->color);
					}
					else {
						static glm::vec4 no_square_color;
						no_square_color = glm::vec4{};	// Reset to show that it is not tweaking anything
						return glm::value_ptr(no_square_color);
					}
				});
			ImGui::ColorEdit4("Square Color", color);
		}

		{
        	if (ImGui::BeginListBox("Names")) {
				if (square_is_valid) {
					auto names = square.components<component::Name>();
					SAGE_ASSERT(names.has_value());
					std::apply(
							[&] (const auto&... ns) {
								(
								 std::invoke([&, n = 0] mutable {
										SAGE_ASSERT(ns.has_value());

										static int item_current_idx = 0; // Here we store our selection data as an index.
										static bool is_selected = false;
										if (ImGui::Selectable(ns->name.c_str(), &is_selected))
											item_current_idx = n;

										// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
										if (is_selected) {
											ImGui::SetItemDefaultFocus();

											ecs.destroy(square);
											is_selected = false;
										}

										++n;
									})
								 , ...
								);
							},
							*names
						);
				}

        	    ImGui::EndListBox();
        	}
		}

		if (ImGui::Button("Recreate Square")) {
			if (not square.is_valid()) {
				SAGE_ASSERT(not ecs.is_full());
				square = *ecs.create();
				square.set(component::Name{"Square"s}, component::Sprite{}, component::Transform{}, component::Camera{}, component::Position{});

				cam.set_position(std::get<0>(*square.components<component::Position>())->position);
			}
		}

		ImGui::SameLine();
		ImGui::Text(square.is_valid() ? "Zoom from square" : "Zoom from mouse scroll");

		// Components
		if (square.is_valid()) {

			auto comps = square.components();
			SAGE_ASSERT(comps.has_value());

			if (ImGui::BeginTable("Components", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
				std::apply(
						[&] (auto&&... cs) {
						(
							std::invoke([&] {
									using Cs = std::decay_t<decltype(cs)>::value_type;

									ImGui::TableNextRow();

									ImGui::TableNextColumn();
									ImGui::PushItemWidth(ImGui::CalcTextSize("A").x * 3.0f); // Small
									ImGui::Text("%s", Cs::type_name().data());

									ImGui::TableNextColumn();
									ImGui::Text("%s", fmt::format("{}", cs).c_str());
								})
							, ...
						);
						},
						*comps
					);

				ImGui::EndTable();
			}

			auto& cam_comp = std::get<std::optional<component::Camera>&>(*comps);
			SAGE_ASSERT(cam_comp.has_value());

			auto size = cam_comp->camera.size();
			ImGui::DragFloat("Square Camera Size", &size, 0.25f, 0.25f, 30.f);
			cam_comp->camera.set_size(size);

			cam.set_zoom(size);
		}

		ImGui::End();
	}
};

struct Game_State {
	Level level;
	bool should_update = true;

	Game_State(ECS& ecs)
		: level{ecs}
	{}
};

struct Layer_2D {
	using Input = oslinux::Input;
	using Renderer = oslinux::Renderer_2D;
	using User_State = Game_State;

public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input, camera::Controller<Input>&, ECS&, Game_State& gs) -> void {
		if (gs.should_update)
			gs.level.update(delta, input);
	}

	auto render(oslinux::Renderer_2D& renderer, ECS&, Game_State& gs) -> void {
		gs.level.render(renderer);
	}

	auto event_callback(const Event& e, camera::Controller<Input>&, ECS&, Game_State& gs) -> void {
		gs.should_update = toogle_if(gs.should_update, e.type == Event::Type::Key_Pressed and std::get<input::Key>(e.payload) == input::Key::P);
	}

	auto imgui_prepare(camera::Controller<Input>& cam, Renderer::Frame_Buffer&, ECS& ecs, Game_State& gs) -> void {
		gs.level.imgui_prepare(cam, ecs);
	}

};

struct Rocket_Flame : particle::system::Base {
	using Base = particle::system::Base;
	using Input = oslinux::Input;
	using Renderer = oslinux::Renderer_2D;
	using User_State = Game_State;

public:
	Rocket_Flame(const size_t max_particles = 100)
		: Base{max_particles}
	{}

public:
	auto update(const std::chrono::milliseconds dt, oslinux::Input& input, camera::Controller<Input>&, ECS&, Game_State& gs) -> void {
		if (not gs.should_update)
			return;

		const auto position = glm::vec3{};;
		const auto rotation = 0.f;
		constexpr auto arc = 9.f;
		const auto direction = glm::rotate(
				identity<glm::vec2>,
				glm::radians(random::in_range(rotation - arc, rotation + arc)) - 90.f
			);
		const auto lifetime = std::chrono::milliseconds{random::in_range(250, 500)};
		const auto color = glm::vec4{random::normalized(), random::normalized(), random::normalized(), 1.f};

		if (input.is_key_pressed(input::Key::Space)) {
			constexpr auto speed = 0.3f;

			Base::emit(particle::Properties{
					.position = position,
					.velocity = speed * direction, //{random::in_range(-0.25f, 0.25f), random::in_range(-0.25f, 0.25f)},
					.color = color,
					.size = random::in_range(0.25f, 0.75f),
					.lifetime = lifetime
				});
		}
		else if (random::toggle::rare()) {
			constexpr auto speed = 0.1f;

			Base::emit(particle::Properties{
					.position = position,
					.velocity = speed * direction, //{random::in_range(-0.25f, 0.25f), random::in_range(-0.25f, 0.25f)},
					.color = color,
					.size = random::in_range(0.1f, 0.25f),
					.lifetime = lifetime
				});
		}

		Base::update(dt, [] (auto& particles) {
				rg::for_each(particles, [] (auto& p) {
						p.properties.position += p.properties.velocity;

						p.properties.color += 0.035f;

						glm::clamp(p.properties.color, {0.f, 0.f, 0.f, 0.f}, {1.f, 1.f, 1.f, 1.f});

						const auto rotate_clockwise = random::between(-1, 1);
						p.properties.rotation += rotate_clockwise * random::in_range(0.f, 25.f);
					});
			});
	}

	auto render(oslinux::Renderer_2D& renderer, ECS&, Game_State&) -> void {
		Base::render([&] (const auto& particles) {
				rg::for_each(particles, [&] (const auto& p) {
						using Simple_Args = oslinux::Renderer_2D::Simple_Args;
						renderer.draw(p.properties.color, Simple_Args{
								.position = {p.properties.position, 1.f},
								.size = {p.properties.size, p.properties.size},
								.rotation = p.properties.rotation
							});
					});
			});
	}

	auto event_callback(const Event&, camera::Controller<Input>&, ECS&, Game_State&) -> void {
	}

	auto imgui_prepare(camera::Controller<Input>&, Renderer::Frame_Buffer&, ECS&, Game_State&) -> void {
	}

	FMT_FORMATTER(Rocket_Flame);
};

template <>
FMT_FORMATTER(Layer_2D) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Layer_2D) {
		(void)obj;
		return fmt::format_to(ctx.out(), "Layer_2D;");
	}
};

template <>
FMT_FORMATTER(Rocket_Flame) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Rocket_Flame) {
		(void)obj;
		return fmt::format_to(ctx.out(), "Rocket_Flame;");
	}
};
