#pragma once

#include "Third/glm/glm.hpp"
#include "Third/glm/gtc/quaternion.hpp"

namespace Mrk
{
	struct Vector2 : public glm::vec2
	{
		using glm::vec2::vec2;
	};

	struct Vector3 : public glm::vec3
	{
		using glm::vec3::vec3;
	};

	struct Vector4 : public glm::vec4
	{
		using glm::vec4::vec4;
	};

	struct Vector2i : public glm::ivec2
	{
		using glm::ivec2::ivec2;
	};

	struct Vector3i : public glm::ivec3
	{
		using glm::ivec3::ivec3;
	};

	struct Vector4i : public glm::ivec4
	{
		using glm::ivec4::ivec4;
	};

	struct Quaternion : public glm::quat
	{
		using glm::quat::quat;
	}; 

	struct Matrix4 : public glm::mat4
	{
		using glm::mat4::mat4;

		float GetM00() const { return (*this)[0][0]; }
		float GetM01() const { return (*this)[0][1]; }
		float GetM02() const { return (*this)[0][2]; }
		float GetM03() const { return (*this)[0][3]; }
		float GetM10() const { return (*this)[1][0]; }
		float GetM11() const { return (*this)[1][1]; }
		float GetM12() const { return (*this)[1][2]; }
		float GetM13() const { return (*this)[1][3]; }
		float GetM20() const { return (*this)[2][0]; }
		float GetM21() const { return (*this)[2][1]; }
		float GetM22() const { return (*this)[2][2]; }
		float GetM23() const { return (*this)[2][3]; }
		float GetM30() const { return (*this)[3][0]; }
		float GetM31() const { return (*this)[3][1]; }
		float GetM32() const { return (*this)[3][2]; }
		float GetM33() const { return (*this)[3][3]; }

		void SetM00(float val) { (*this)[0][0] = val; }
		void SetM01(float val) { (*this)[0][1] = val; }
		void SetM02(float val) { (*this)[0][2] = val; }
		void SetM03(float val) { (*this)[0][3] = val; }
		void SetM10(float val) { (*this)[1][0] = val; }
		void SetM11(float val) { (*this)[1][1] = val; }
		void SetM12(float val) { (*this)[1][2] = val; }
		void SetM13(float val) { (*this)[1][3] = val; }
		void SetM20(float val) { (*this)[2][0] = val; }
		void SetM21(float val) { (*this)[2][1] = val; }
		void SetM22(float val) { (*this)[2][2] = val; }
		void SetM23(float val) { (*this)[2][3] = val; }
		void SetM30(float val) { (*this)[3][0] = val; }
		void SetM31(float val) { (*this)[3][1] = val; }
		void SetM32(float val) { (*this)[3][2] = val; }
		void SetM33(float val) { (*this)[3][3] = val; }
	}; 
}
