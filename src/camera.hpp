#pragma once

#include "std.hpp"

#include "repr.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/string_cast.hpp>

namespace sage::camera {

// TODO: Not sure if this API is necessary, maybe have public members?
struct Orthographic {

private:
	glm::mat4 _projection_mat;
	glm::mat4 _view_mat;
	glm::mat4 _view_proj_mat;	// Cached
	glm::vec3 _position;
	float _rotation;			// In degrees TODO: Make a separete unit::Degrees struct?

public:
	struct Args {
		float&& left, right, bottom, top;
	};
	Orthographic(Args&& args)
		: _projection_mat{glm::ortho(args.left, args.right, args.bottom, args.top, -1.f, 1.f)}
		, _view_mat{1.f}
		, _position{0, 0, 0}
		, _rotation{0.f}
	{
		calc_view_mat();
	}

	Orthographic(Orthographic&&) = default;

public:
	auto position() -> const glm::vec3& {
		return _position;
	}

	auto set_position(const glm::vec3& pos) -> void {
		_position = pos;
		calc_view_mat();
	}

	auto rotation() -> float {
		return _rotation;
	}

	auto set_rotation(const float rot) -> void {
		_rotation = rot;
		calc_view_mat();
	}

	auto projection_mat() -> const glm::mat4& {
		return _projection_mat;
	}

	auto view_mat() -> const glm::mat4& {
		return _view_mat;
	}

	auto view_proj_mat() -> const glm::mat4& {
		return _view_proj_mat;
	}


private:
	auto calc_view_mat() -> void {
		const auto transform =
				glm::translate(glm::mat4(1.f), _position)
				* glm::rotate(glm::mat4(1.f), glm::radians(_rotation), glm::vec3(0, 0, 1)
			);

		_view_mat = glm::inverse(transform);
		_view_proj_mat = _projection_mat * _view_mat;
	}

public:
	friend FMT_FORMATTER(Orthographic);
};

}// sage::camera

template <>
FMT_FORMATTER(sage::camera::Orthographic) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::camera::Orthographic) {
		return fmt::format_to(ctx.out(), "camera::Orthographic: position={}; rotation={};;", glm::to_string(obj._position), obj._rotation);
	}
};
