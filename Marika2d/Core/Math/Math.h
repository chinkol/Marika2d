#pragma once

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL

#include "Third/glm/glm.hpp"
#include "Third/glm/gtc/quaternion.hpp"
#include "Third/glm/gtx/matrix_decompose.hpp"
#include "Third/glm/gtc/matrix_transform.hpp"
#include "Third/glm/gtc/type_ptr.hpp"
#include "Third/glm/gtx/quaternion.hpp"

namespace Mrk
{
	using Vector2 = glm::vec2;
	using Vector3 = glm::vec3;
	using Vector4 = glm::vec4;

	using Vector2i = glm::ivec2;
	using Vector3i = glm::ivec3;
	using Vector4i = glm::ivec4;

	using Quaternion = glm::quat;
	using Matrix4 = glm::mat4;
}
