#pragma once

#include "src/std.hpp"

#include "src/repr.hpp"

#include "src/math.hpp"

#include "src/event.hpp"
#include "src/input.hpp"

namespace sage::camera {

#pragma message "TODO: Not sure if this API is necessary, maybe have public members?"
struct Orthographic {
	template <input::Concept>
	friend struct Controller;

private:
	glm::mat4 _projection_mat;
	glm::mat4 _view_mat;
	glm::mat4 _view_proj_mat;	// Cached

public:
	struct Projection_Args {
		float left, right, bottom, top;
	};
	Orthographic(Projection_Args&& args)
		: _projection_mat{glm::ortho(args.left, args.right, args.bottom, args.top, -1.f, 1.f)}
		, _view_mat{1.f}
	{
		_view_proj_mat = _projection_mat * _view_mat;
	}

	Orthographic(Orthographic&&) = default;

public:
	auto set_projection(Projection_Args&& args) {
		_projection_mat = glm::ortho(args.left, args.right, args.bottom, args.top, -1.f, 1.f);
		_view_proj_mat = _projection_mat * _view_mat;
	}

public:
	auto projection_mat() const -> const glm::mat4& {
		return _projection_mat;
	}

	auto view_mat() const -> const glm::mat4& {
		return _view_mat;
	}

	auto view_proj_mat() const -> const glm::mat4& {
		return _view_proj_mat;
	}

public:
	friend FMT_FORMATTER(Orthographic);
};

template <input::Concept Input>
struct Controller {
	struct Zoom {
		float min,
			  max,
			  level;

	};

	struct Rotation {
		// In degrees TODO: Make a separete unit::Degrees struct?
		float level,
			  speed;
	};

private:
	float aspect_ratio;
	Zoom zoom;
	glm::vec3 position;
	float move_speed;
	Rotation rotation;
	camera::Orthographic _camera;

public:
	struct Controller_Args {
		float aspect_ratio = 16.f / 9.f;
		Zoom zoom = {
				.min = 0.25f,
				.max = 10.f,
				.level = 10.f
			};
		glm::vec3&& position = {0, 0, 0};
		Rotation rotation = {
				.level = 0,
				.speed = 180.f
			};
		float move_speed = zoom.level;
	};
	Controller(Controller_Args&& args = {})
		: aspect_ratio{args.aspect_ratio}
		, zoom{args.zoom}
		, position{args.position}
		, move_speed{args.move_speed}
		, rotation{args.rotation}
		, _camera{projection_mat_args()}
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

		calc_view_mat();
	}

	auto event_callback(const Event& e) -> void {
		switch (e.type) {
			case Event::Type::Mouse_Scrolled:
				SAGE_ASSERT(std::holds_alternative<input::Mouse::Scroll>(e.payload));

				zoom.level = std::clamp(zoom.level - 0.5 * std::get<input::Mouse::Scroll>(e.payload).offset.y, 0.25, 30.0);
				move_speed = zoom.level;
				break;
			case Event::Type::Window_Resized: {
				SAGE_ASSERT(std::holds_alternative<Size<size_t>>(e.payload));

				const auto sz = std::get<Size<size_t>>(e.payload).to<float>();
				aspect_ratio = sz.width / sz.height;
				break;
			}
			default:
				return;
		}

		_camera.set_projection(projection_mat_args());
	}

private:
	auto calc_view_mat() -> void {
		const auto transform =
				glm::translate(glm::mat4(1.f), position)
				* glm::rotate(glm::mat4(1.f), glm::radians(rotation.level), glm::vec3(0, 0, 1)
			);

		_camera._view_mat = glm::inverse(transform);
		_camera._view_proj_mat = _camera._projection_mat * _camera._view_mat;
	}

	auto projection_mat_args() const -> camera::Orthographic::Projection_Args {
		return {
				.left	= -aspect_ratio * zoom.level,
				.right	= aspect_ratio * zoom.level,
				.bottom	= -zoom.level,
				.top	= zoom.level,
			};
	}

public:
	auto camera() const -> const Orthographic& {
		return _camera;
	}

public:
	friend FMT_FORMATTER(Controller);
};

}// sage::camera

template <>
FMT_FORMATTER(sage::camera::Orthographic) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::camera::Orthographic) {
		return fmt::format_to(ctx.out(), "camera::Orthographic: projection_mat={};;", glm::to_string(obj._projection_mat));
	}
};

template <sage::input::Concept I>
FMT_FORMATTER(sage::camera::Controller<I>) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::camera::Controller<I>) {
		return fmt::format_to(ctx.out(), "camera::Controller: position={}; rotation={}; camera={};", glm::to_string(obj.position), obj.rotation.level, obj._camera);
	}
};
