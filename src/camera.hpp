#pragma once

#include "src/std.hpp"

#include "src/repr.hpp"

#include "src/math.hpp"

#include "src/event.hpp"
#include "src/input.hpp"

namespace sage::camera {

// RND: Look into the details for cameras projection math
struct Camera {
	glm::mat4 projection;

	struct Orthographic_Args {
		float left, right, bottom, top;
	};
	static constexpr auto orthographic(Orthographic_Args&& args) -> Camera {
		return { glm::ortho(args.left, args.right, args.bottom, args.top, -1.f, 1.f) };
	}
};

struct Scene_Camera : Camera {
private:
	float aspect_ratio = 0.f,
		  _size = 10.f,
		  near = -1.f, far = 1.f;

public:
	auto set_viewport_size(const glm::vec2& vp) -> void {
		aspect_ratio = vp.x / vp.y;

		calc_projection();
	}

	auto size() const -> float {
		return _size;
	}

	auto set_size(const float s) -> void {
		_size = s;
		calc_projection();
	}

private:
	auto calc_projection() -> void {
		projection = glm::ortho(
				-_size * aspect_ratio * 0.5f,
				_size * aspect_ratio * 0.f,
				-_size * 0.5f,
				_size * 0.5f,
				near,
				far
			);
	}
};

// Keep the comments for a bit to save the math
template <input::Concept Input>
struct Controller {
	struct Zoom {
		float min,
			  max,
			  level;

	};

	struct Rotation {
		// TODO: Make a separete unit::Degrees struct?
		// In degrees
		float level,
			  speed;
	};

public:
	camera::Camera camera;

private:
	float aspect_ratio;
	Zoom _zoom;
	glm::vec3 position;
	float move_speed;
	Rotation rotation;

	glm::mat4 transform;

public:
	struct Controller_Args {
		float aspect_ratio = 16.f / 9.f;
		Zoom zoom = {
				.min = 0.25f,
				.max = 10.f,
				.level = 10.f
			};
		glm::vec3 position = {0, 0, 0};
		Rotation rotation = {
				.level = 0,
				.speed = 180.f
			};
		float move_speed = zoom.level;
	};
	Controller(Controller_Args&& args = {})
		: aspect_ratio{args.aspect_ratio}
		, _zoom{args.zoom}
		, position{args.position}
		, move_speed{args.move_speed}
		, rotation{args.rotation}
		, camera{Camera::orthographic(orthographic_args())}
	{}

public:
	auto update(const std::chrono::milliseconds dt, Input& input) -> void {
		const auto dt_coeff = std::chrono::duration<float, std::chrono::seconds::period>{dt}.count();

		if		(input.is_key_pressed(input::Key::W))	position.y += move_speed * dt_coeff;
		else if (input.is_key_pressed(input::Key::S))	position.y -= move_speed * dt_coeff;

		if		(input.is_key_pressed(input::Key::D))	position.x += move_speed * dt_coeff;
		else if (input.is_key_pressed(input::Key::A))	position.x -= move_speed * dt_coeff;

		if		(input.is_key_pressed(input::Key::Q))	rotation.level -= rotation.speed * dt_coeff;
		else if	(input.is_key_pressed(input::Key::E))	rotation.level += rotation.speed * dt_coeff;

		calc_transform();
	}

	auto event_callback(const Event& e) -> void {
		switch (e.type) {
			case Event::Type::Mouse_Scrolled: {
				SAGE_ASSERT(std::holds_alternative<input::Mouse::Scroll>(e.payload));

				const auto offset = std::get<input::Mouse::Scroll>(e.payload).offset;
				zoom({offset.x, offset.y});
				break;
			}

			case Event::Type::Window_Resized: {
				SAGE_ASSERT(std::holds_alternative<Size<size_t>>(e.payload));

				// TODO: Only use glm:: instead of the ad-hoc math::Size
				const auto sz = std::get<Size<size_t>>(e.payload).to<float>();
				resize({sz.width, sz.height});
				break;
			}

			default:
				return;
		}
	}

public:
	auto set_position(const glm::vec3& pos) -> void {
		position = pos;

		calc_transform();
	}

	auto zoom(const glm::vec2& offset) -> void {
		_zoom.level = std::clamp(_zoom.level - 0.5 * offset.y, 0.25, 30.0);
		move_speed = _zoom.level;

		camera = Camera::orthographic(orthographic_args());
		camera.projection *= transform;
	}

	auto set_zoom(const float& z) -> void {
		_zoom.level = std::clamp(z, 0.25f, 30.0f);

		camera = Camera::orthographic(orthographic_args());
		camera.projection *= transform;
	}

	auto resize(const glm::vec2& sz) -> void {
		aspect_ratio = sz.x / sz.y;

		camera = Camera::orthographic(orthographic_args());
		camera.projection *= transform;
	}

public:
	static constexpr auto null() -> Controller& {
		static auto c = Controller{};
		return c;
	}

	auto orthographic_args() const -> Camera::Orthographic_Args {
		return {
			.left	= -aspect_ratio * _zoom.level,
			.right	= aspect_ratio * _zoom.level,
			.bottom	= -_zoom.level,
			.top	= _zoom.level,
		};
	}

private:
	auto calc_transform() -> void {
		transform =
				glm::inverse(
					glm::translate(glm::mat4(1.f), position)
					* glm::rotate(glm::mat4(1.f), glm::radians(rotation.level), glm::vec3(0, 0, 1)
				)
			);
	}

public:
	friend FMT_FORMATTER(Controller);
};


}// sage::camera

template <>
FMT_FORMATTER(sage::camera::Camera) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::camera::Camera) {
		return fmt::format_to(ctx.out(), "camera::Camera: {};", glm::to_string(obj.projection));
	}
};

template <sage::input::Concept I>
FMT_FORMATTER(sage::camera::Controller<I>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::camera::Controller<I>) {
		return fmt::format_to(ctx.out(), "camera::Controller: position={}; rotation={}; camera={};", glm::to_string(obj.position), obj.rotation.level, obj.camera);
	}
};
