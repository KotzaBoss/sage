#pragma once

#include "src/sage.hpp"

#include "src/platform/linux/sage.hpp"

using namespace sage;

struct Player {
private:
	// TODO: What to encapsulate, player.update(), player knows renderer?
	glm::vec3 _position = {0.f, 0.f, 0.f};
	glm::vec2 _velocity = {0.f, 0.f};

	float engine_power = 2.f;
	float gravity = 0.8f;

	oslinux::Texture2D texture = {"asset/texture/Ship.png"};

public:
	auto update([[maybe_unused]] const std::chrono::milliseconds delta, oslinux::Input& input) {
		if (input.is_key_pressed(input::Key::Space))
			_velocity.y += engine_power;
		else
			_velocity.y -= gravity;

		_velocity.y = glm::clamp(_velocity.y, -20.f, 20.f);
		const auto dt_coeff = std::chrono::duration<float, std::chrono::seconds::period>{delta}.count();
		_position += glm::vec3(_velocity, 0.f) * dt_coeff;
	}

	auto render(oslinux::Renderer_2D& renderer) {
		using Simple_Args = oslinux::Renderer_2D::Simple_Args;
		renderer.draw(texture, Simple_Args{
				.position = _position,
				.size = {1.0f, 1.3f},
				.rotation = 3.f * _velocity.y - 90.f,
			});
	}

	auto imgui_prepare() {
		::ImGui::DragFloat("Engine Power", &engine_power, 0.05f);
		::ImGui::DragFloat("Gravity", &gravity, 0.01f);
	}

public:
	auto position() const -> const glm::vec3& {
		return _position;
	}

	auto velocity() const -> const glm::vec2& {
		return _velocity;
	}

	auto rotation() const -> float {
		return _velocity.y - 90.f;
	}
};

struct Obstacle {

private:
	glm::vec3 _position;
	glm::vec2 size;
	float rotation;

	float move_speed;
	float rotation_speed;

public:
	Obstacle(glm::vec2&& p, glm::vec2&& sz, const float m_speed = 10.f, const float r_speed = 0.f)
		: _position{std::move(p), 0.f}
		, size{std::move(sz)}
		, rotation{0.f}
		, move_speed{m_speed}
		, rotation_speed{r_speed}
	{}

	Obstacle(Obstacle&&) = default;
	auto operator= (Obstacle&&) -> Obstacle& = default;

public:
	auto update(const std::chrono::milliseconds delta, input::Concept auto&) {
		const auto dt_coeff = std::chrono::duration<float, std::chrono::seconds::period>{delta}.count();

		_position.x -= move_speed * dt_coeff;
		rotation += rotation_speed * dt_coeff;
	}

	auto render(oslinux::Renderer_2D& renderer) {
		using Simple_Args = oslinux::Renderer_2D::Simple_Args;
		renderer.draw(glm::vec4{1.f, 0, 0, 1.f}, Simple_Args{ .position = _position, .size = size, .rotation = rotation });
	}

	auto position() const -> const glm::vec3& {
		return _position;
	}
};

struct Level {
private:
	Player _player;

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

	static constexpr auto map = std::array{
			"WWWWWWWWWWWWWWWWWWWWWWW"sv,
			"WWWWWWWDDDDDWWWWWWWWWWW"sv,
			"WWWWWDDDDDDDDDDDDWWWWWW"sv,
			"WWWWWDDDDDDDDDDWWWWWWWW"sv,
			"WWWWDDDDDDDDDDDDDDWWWWW"sv,
			"WWWWDDDDDDDDDDDDDWWWDWW"sv,
			"WWDDDWWWWDDDDDDDWWWDWWW"sv,
			"WWDDDDWWWDDDDDDWWWWDDWW"sv,
			"WWDDWDDWWDDDDDDWWWDDDDW"sv,
			"WDWWWWDDDWWWDDDWWWWWWWW"sv,
			"WWWWWWWWWWWWWWWWWWWWWWW"sv,
		};

private:
	using ECS = sage::ECS<component::Name, component::Sprite, component::Transform>;
	ECS ecs;
	ECS::Entity square;

public:
	Level()
		: ecs{1ul}
	{}

public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input) {
	}

	auto render(oslinux::Renderer_2D& renderer) {
		using Simple_Args = oslinux::Renderer_2D::Simple_Args;

		constexpr auto size = glm::vec2{ 1.f, 1.f };

		SAGE_ASSERT(rg::all_of(map, [len = map.front().size()] (const auto& str) { return str.size() == len; }));

		for (const auto y : vw::iota(0ul, map.size())) {
			const auto& str = map[y];
			for (const auto x : vw::iota(0ul, str.size())) {
				switch (str[x]) {
					// size() - y to make sure the map is rendered correctly, otherwise its upside down
					case 'W': renderer.draw(water, Simple_Args{ .position={x, map.size() - y, 0.f}, .size={1, 1} });
						break;
					case 'D': renderer.draw(dirt, Simple_Args{ .position={x, map.size() - y, 0.f}, .size={1, 1} });
						break;
					default: SAGE_DIE();
				}
			}
		}

		if (square.is_valid()) {
			auto comps = square.components();
			SAGE_ASSERT(comps.has_value());

			auto& sprite = std::get<std::optional<component::Sprite>&>(*comps);
			auto& transform = std::get<std::optional<component::Transform>&>(*comps);
			SAGE_ASSERT(sprite.has_value());
			SAGE_ASSERT(transform.has_value());

			renderer.draw(sprite->color, transform->trans);
		}
	}

	auto imgui_prepare() {
		ImGui::Begin("Level");

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
				const auto ok = ecs.create(square);
				SAGE_ASSERT(ok);
				square.set(component::Name{"Square"s}, component::Sprite{}, component::Transform{});
			}
		}

		ImGui::End();
	}

public:
	auto player() const -> const Player& {
		return _player;
	}
};

struct Game_State {
	Level level;
	bool should_update = true;
};

struct Layer_2D {
	using Input = oslinux::Input;
	using Renderer = oslinux::Renderer_2D;
	using User_State = Game_State;

public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input, camera::Controller<Input>&, Game_State& gs) -> void {
		if (gs.should_update)
			gs.level.update(delta, input);
	}

	auto render(oslinux::Renderer_2D& renderer, Game_State& gs) -> void {
		gs.level.render(renderer);
	}

	auto event_callback(const Event& e, camera::Controller<Input>&, Game_State& gs) -> void {
		gs.should_update = toogle_if(gs.should_update, e.type == Event::Type::Key_Pressed and std::get<input::Key>(e.payload) == input::Key::P);
	}

	auto imgui_prepare(camera::Controller<Input>&, Renderer::Frame_Buffer&, Game_State& gs) -> void {
		gs.level.imgui_prepare();
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
	auto update(const std::chrono::milliseconds dt, oslinux::Input& input, camera::Controller<Input>&, Game_State& gs) -> void {
		if (not gs.should_update)
			return;

		const auto& player = gs.level.player();
		const auto position = player.position();
		const auto rotation = player.rotation();
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

	auto render(oslinux::Renderer_2D& renderer, Game_State&) -> void {
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

	auto event_callback(const Event&, camera::Controller<Input>&, Game_State&) -> void {
	}

	auto imgui_prepare(camera::Controller<Input>&, Renderer::Frame_Buffer&, Game_State&) -> void {
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
