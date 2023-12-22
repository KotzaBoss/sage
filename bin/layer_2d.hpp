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
		renderer.draw(texture, {
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
		renderer.draw(glm::vec4{1.f, 0, 0, 1.f}, { .position = _position, .size = size, .rotation = rotation });
	}

	auto position() const -> const glm::vec3& {
		return _position;
	}
};

struct Level {
private:
	Player _player;
	// Conceptually the vector has the same "direction" as the incoming obstacles meaning
	// the front() is the left-most obstacle
	std::vector<Obstacle> obstacles;

	static constexpr auto obstacle_distance = 20.f;
	static constexpr auto max_obstacles = 100;
	static constexpr auto rand_y = [] { return glm::linearRand(-5.f, 5.f); };

public:
	Level() {
		obstacles.reserve(max_obstacles);
		rg::generate_n(std::back_inserter(obstacles), obstacles.capacity(),
				[&] mutable {
					return Obstacle{glm::vec2{obstacles.size() * obstacle_distance, rand_y()}, glm::vec2{1.f, 1.f}, 20.f, 50.f};
				}
			);
	}

public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input) {
		_player.update(delta, input);
		rg::for_each(obstacles, [&] (auto& o) { o.update(delta, input); });
	}

	auto render(oslinux::Renderer_2D& renderer) {
		constexpr auto border_color = glm::vec4{0.25f, 0.5f, 0.1f, 1.f};

		renderer.draw(border_color, {
				.position = {_player.position().x, 10.f, 0.f},
				.size = {50.f, 10.f},
			});

		constexpr auto right = 5.f;
		for (auto x = -right; x < right; x += 0.5f)
			for (auto y = -right; y < right; y += 0.5f)
			{
				renderer.draw(
						glm::vec4{ (x + right) / 10.f, 0.4f, (y + right) / 10.f, 0.5f },
						{
							.position = {x, y, 0.f},
							.size = {0.35f, 0.35f}
						}
					);
			}

		rg::for_each(obstacles, [&] (auto& o) { o.render(renderer); });
		_player.render(renderer);

		SAGE_ASSERT(not obstacles.empty());
		SAGE_ASSERT(obstacles.capacity() == max_obstacles and obstacles.size() == max_obstacles);
		if (obstacles.front().position().x < -30.f) {
			rg::rotate(obstacles, obstacles.begin() + 1);
			obstacles.back() = Obstacle{
					glm::vec2{(obstacles.rbegin() + 1)->position().x + obstacle_distance, rand_y()},
					glm::vec2{1.f, 1.f},
					20.f,
					50.f
				};
		}

		renderer.draw(border_color, {
				.position = {_player.position().x, -10.f, 0.f},
				.size = {50.f, 10.f},
			});
	}

	auto imgui_prepare() {
		::ImGui::Begin("Player");
		_player.imgui_prepare();
		::ImGui::End();
	}

public:
	auto player() const -> const Player& {
		return _player;
	}
};

struct Game_State {
	Level level;
};

struct Layer_2D {
public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input, Game_State& game_state) -> void {
		game_state.level.update(delta, input);
	}

	auto render(oslinux::Renderer_2D& renderer, Game_State& game_state) -> void {
		game_state.level.render(renderer);
	}

	auto event_callback(const Event&, Game_State&) -> void {
	}

	auto imgui_prepare() -> void {
		//level.imgui_prepare();
	}

};

struct Rocket_Flame : particle::system::Base {
	using Base = particle::system::Base;

public:
	Rocket_Flame(const size_t max_particles = 100)
		: Base{max_particles}
	{}

public:
	auto update(const std::chrono::milliseconds, oslinux::Input& input, Game_State& game_state) -> void {
		const auto& player = game_state.level.player();
		const auto position = player.position();
		const auto rotation = player.rotation();
		constexpr auto arc = 7.f;
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

		Base::update([] (auto& particles) {
				rg::for_each(particles, [] (auto& p) {
						p.properties.position += p.properties.velocity;
						p.properties.color += 0.035f;
						glm::clamp(p.properties.color, {0.f, 0.f, 0.f, 0.f}, {1.f, 1.f, 1.f, 1.f});
					});
			});
	}

	auto render(oslinux::Renderer_2D& renderer, Game_State&) -> void {
		Base::render([&] (const auto& particles) {
				rg::for_each(particles, [&] (const auto& p) {
						renderer.draw(p.properties.color, {
								.position = {p.properties.position, 1.f},
								.size = {p.properties.size, p.properties.size},
							});
					});
			});
	}

	auto event_callback(const Event&, Game_State&) -> void {
	}

	auto imgui_prepare() -> void {
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
