#include "GenCoreReflectInfo.h"

#include "Core/Reflect/Reflect.h"

#include <vector>
#include <functional>

namespace
{
	std::vector<std::function<void()>> core_reflect_registers;

	void CoreTypeReflectRegister()
	{
		for (auto& core_reflect_register : core_reflect_registers)
		{
			if (core_reflect_register)
			{
				core_reflect_register();
			};
		}
	}
}

namespace
{
	void BasicTypeRegister()
	{
		MRK_RTTR_BASIC_TYPE(char);
		MRK_RTTR_BASIC_TYPE(int8_t);
		MRK_RTTR_BASIC_TYPE(short);
		MRK_RTTR_BASIC_TYPE(int16_t);
		MRK_RTTR_BASIC_TYPE(int32_t);
		MRK_RTTR_BASIC_TYPE(long long);
		MRK_RTTR_BASIC_TYPE(intptr_t);
		MRK_RTTR_BASIC_TYPE(int64_t);
		MRK_RTTR_BASIC_TYPE(unsigned short);
		MRK_RTTR_BASIC_TYPE(uint16_t);
		MRK_RTTR_BASIC_TYPE(unsigned int);
		MRK_RTTR_BASIC_TYPE(uint32_t);
		MRK_RTTR_BASIC_TYPE(unsigned long long);
		MRK_RTTR_BASIC_TYPE(size_t);
		MRK_RTTR_BASIC_TYPE(uint64_t);
		MRK_RTTR_BASIC_TYPE(float);
		MRK_RTTR_BASIC_TYPE(float_t);
		MRK_RTTR_BASIC_TYPE(double);
		MRK_RTTR_BASIC_TYPE(double_t);
		MRK_RTTR_BASIC_TYPE(std::string);
	}
}

#include "Third/glm/glm.hpp"
#include "Third/glm/gtc/quaternion.hpp"

namespace
{
	void GlmReflectRegister()
	{
		rttr::registration::class_<glm::vec2>("glm::vec2")
			.constructor<>()(rttr::policy::ctor::as_object)
			.property("x", &glm::vec2::x)
			.property("y", &glm::vec2::y);

		rttr::registration::class_<glm::vec3>("glm::vec3")
			.constructor<>()(rttr::policy::ctor::as_object)
			.property("x", &glm::vec3::x)
			.property("y", &glm::vec3::y)
			.property("z", &glm::vec3::z);

		rttr::registration::class_<glm::vec4>("glm::vec4")
			.constructor<>()(rttr::policy::ctor::as_object)
			.property("x", &glm::vec4::x)
			.property("y", &glm::vec4::y)
			.property("z", &glm::vec4::z)
			.property("w", &glm::vec4::w);

		rttr::registration::class_<glm::quat>("glm::quat")
			.constructor<>()(rttr::policy::ctor::as_object)
			.property("x", &glm::quat::x)
			.property("y", &glm::quat::y)
			.property("z", &glm::quat::z)
			.property("w", &glm::quat::w);

		rttr::registration::class_<glm::mat4>("glm::mat4")
			.constructor<>()(rttr::policy::ctor::as_object);
	}
}

void Mrk::GenCoreReflectInfo()
{
	BasicTypeRegister();
	GlmReflectRegister();

	// last
	CoreTypeReflectRegister();
}

#ifndef MRK_CORE_REFLECT_REGISTER_CONTENT
#define MRK_CORE_REFLECT_REGISTER_CONTENT(x) x }); return true; }();
#endif // !MRK_CORE_REFLECT_REGISTER_CONTENT

#ifndef MRK_CORE_REFLECT_REGISTER
#define MRK_CORE_REFLECT_REGISTER(x) static inline bool _mrk_rttr_core_type_##x##_reflect_register_ = [](){core_reflect_registers.push_back([](){ MRK_CORE_REFLECT_REGISTER_CONTENT
#endif // !MRK_CORE_REFLECT_REGISTER

#include "Core/ID/IDCreater.h"

namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(ID)(
		rttr::registration::class_<ID>("ID")
		.constructor<>()
		.property("low32", &ID::low32)
		.property("high32", &ID::high32)
		.property("total64", &ID::total64);
		);
}

#include "Core/Math/Math.h" 

namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(Vector2)(
		rttr::registration::class_<Vector2>("Vector2")
		.constructor<>()
		.property("x", &Vector2::x)
		.property("y", &Vector2::y);
		);

	MRK_CORE_REFLECT_REGISTER(Vector3)(
		rttr::registration::class_<Vector3>("Vector3")
		.constructor<>()
		.property("x", &Vector3::x)
		.property("y", &Vector3::y)
		.property("z", &Vector3::z);
		);

	MRK_CORE_REFLECT_REGISTER(Vector4)(
		rttr::registration::class_<Vector4>("Vector4")
		.constructor<>()
		.property("x", &Vector4::x)
		.property("y", &Vector4::y)
		.property("z", &Vector4::z)
		.property("z", &Vector4::w);
		);

	MRK_CORE_REFLECT_REGISTER(Quaternion)(
		rttr::registration::class_<Quaternion>("Quaternion")
		.constructor<>()
		.property("x", &Quaternion::x)
		.property("y", &Quaternion::y)
		.property("z", &Quaternion::z)
		.property("z", &Quaternion::w);
		);

	MRK_CORE_REFLECT_REGISTER(Matrix4)(
		rttr::registration::class_<Matrix4>("Matrix4")
		.constructor<>()
		.property("m00", &Matrix4::GetM00, &Matrix4::SetM00)
		.property("m01", &Matrix4::GetM01, &Matrix4::SetM01)
		.property("m02", &Matrix4::GetM02, &Matrix4::SetM02)
		.property("m03", &Matrix4::GetM03, &Matrix4::SetM03)
		.property("m10", &Matrix4::GetM10, &Matrix4::SetM10)
		.property("m11", &Matrix4::GetM11, &Matrix4::SetM11)
		.property("m12", &Matrix4::GetM12, &Matrix4::SetM12)
		.property("m13", &Matrix4::GetM13, &Matrix4::SetM13)
		.property("m20", &Matrix4::GetM20, &Matrix4::SetM20)
		.property("m21", &Matrix4::GetM21, &Matrix4::SetM21)
		.property("m22", &Matrix4::GetM22, &Matrix4::SetM22)
		.property("m23", &Matrix4::GetM23, &Matrix4::SetM23)
		.property("m30", &Matrix4::GetM30, &Matrix4::SetM30)
		.property("m31", &Matrix4::GetM31, &Matrix4::SetM31)
		.property("m32", &Matrix4::GetM32, &Matrix4::SetM32)
		.property("m33", &Matrix4::GetM33, &Matrix4::SetM33);
		);
}

#include "Core/GameObject/GameObject.h"

namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(GameObject)(
		rttr::registration::class_<GameObject>("GameObject")
		.constructor<>()
		.property("name", &GameObject::GetName, &GameObject::SetName);
		);

	MRK_CORE_REFLECT_REGISTER(Component)(
		rttr::registration::class_<Component>("Component")
		.constructor<>();
		);
}

#include "Editor/RttrTest.h"

namespace MrkTest
{
	MRK_CORE_REFLECT_REGISTER(BoneComponent)(
		rttr::registration::class_<BoneComponent>("BoneComponent")
		.constructor<>()
		.property("data0", &BoneComponent::data0)
		.property("data1", &BoneComponent::GetData1, &BoneComponent::SetData1)
		.property("data2", &BoneComponent::data2)
		.property("data3", &BoneComponent::data3)
		.property("data4", &BoneComponent::data4);
		);

	MRK_CORE_REFLECT_REGISTER(Bone)(
		rttr::registration::class_<Bone>("Bone")
		.constructor<>();
		);
}
