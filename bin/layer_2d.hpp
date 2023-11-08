#pragma once

#include "sage.hpp"

#include "platform/linux/input.hpp"
#include "platform/linux/graphics.hpp"

using namespace sage;

struct Player {
private:
	// TODO: What to encapsulate, player.update(), player knows renderer?
	glm::vec3 _position = {0.f, 0.f, 0.f};
	glm::vec2 velocity = {0.f, 0.f};

	float engine_power = 2.f;
	float gravity = 0.8f;

	oslinux::Texture2D texture = {"asset/texture/Ship.png"};

public:
	auto update([[maybe_unused]] const std::chrono::milliseconds delta, oslinux::Input& input) {
		if (input.is_key_pressed(input::Key::Space))
			velocity.y += engine_power;
		else
			velocity.y -= gravity;

		velocity.y = glm::clamp(velocity.y, -20.f, 20.f);
		const auto dt_coeff = std::chrono::duration<float, std::chrono::seconds::period>{delta}.count();
		_position += glm::vec3(velocity, 0.f) * dt_coeff;
	}

	auto render(oslinux::Renderer_2D& renderer) {
		renderer.draw(texture, {
				.position = _position,
				.size = {1.0f, 1.3f},
				.rotation = 3.f * velocity.y - 90.f,
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
	Player player;
	// Conceptually the vector has the same "direction" as the incoming obstacles meaning
	// the front() is the left-most obstacle
	std::vector<Obstacle> obstacles;

	static constexpr auto obstacle_distance = 20.f;
	static constexpr auto rand_y = [] { return glm::linearRand(-4.f, 4.f); };

public:
	Level() {
		obstacles.reserve(5);
		rg::generate_n(std::back_inserter(obstacles), obstacles.capacity(),
				[&] mutable {
					return Obstacle{glm::vec2{obstacles.size() * obstacle_distance, rand_y()}, glm::vec2{1.f, 1.f}, 20.f, 50.f};
				}
			);
	}

public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input) {
		player.update(delta, input);
		rg::for_each(obstacles, [&] (auto& o) { o.update(delta, input); });
	}

	auto render(oslinux::Renderer_2D& renderer) {
		renderer.draw(glm::vec4{0.f, 0.f, 0.f, 1.f}, {
				.position = {player.position().x, 10.f, 0.f},
				.size = {50.f, 10.f},
			});

		rg::for_each(obstacles, [&] (auto& o) { o.render(renderer); });
		player.render(renderer);

		SAGE_ASSERT(not obstacles.empty());
		SAGE_ASSERT(obstacles.capacity() == 5 and obstacles.size() == 5);
		if (obstacles.front().position().x < -30.f) {
			rg::rotate(obstacles, obstacles.begin() + 1);
			obstacles.back() = Obstacle{
					glm::vec2{(obstacles.rbegin() + 1)->position().x + obstacle_distance, rand_y()},
					glm::vec2{1.f, 1.f},
					20.f,
					50.f
				};
		}

		renderer.draw(glm::vec4{0.f, 0.f, 0.f, 1.f}, {
				.position = {player.position().x, -10.f, 0.f},
				.size = {50.f, 10.f},
			});
	}

	auto imgui_prepare() {
		::ImGui::Begin("Player");
		player.imgui_prepare();
		::ImGui::End();
	}
};

struct Layer_2D {
	using Camera_Controller = camera::Controller<oslinux::Input>;

public:
	Camera_Controller camera_controller;
	Level level;

public:
	auto update(const std::chrono::milliseconds delta, oslinux::Input& input) -> void {
		camera_controller.update(delta, input);
		level.update(delta, input);
	}

	auto render(oslinux::Renderer_2D& renderer) -> void {
		renderer.clear();

		renderer.scene(camera_controller.camera(), [&] {
				level.render(renderer);
			});
	}

	auto event_callback(const Event& e) -> void {
		camera_controller.event_callback(e);
	}

	auto imgui_prepare() -> void {
		level.imgui_prepare();
	}

};

template <>
FMT_FORMATTER(Layer_2D) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Layer_2D) {
		(void)obj;
		return fmt::format_to(ctx.out(), "Layer_2D;");
	}
};
