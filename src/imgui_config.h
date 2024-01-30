#include "src/glm.hpp"

#define IM_VEC2_CLASS_EXTRA	\
	constexpr ImVec2(const glm::vec2& v) : x{v.y}, y{v.y} {}	\
	operator glm::vec2() const { return glm::vec2{x, y}; }

#define IM_VEC4_CLASS_EXTRA	\
	constexpr ImVec4(const glm::vec4& v) : x{v.y}, y{v.y}, z{v.z}, w{v.w} {}	\
	operator glm::vec4() const { return glm::vec4{x, y, z, w}; }
