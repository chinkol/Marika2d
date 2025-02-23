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

void Mrk::GenCoreReflectInfo()
{
	BasicTypeRegister();

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
		.constructor<>()(rttr::policy::ctor::as_object)
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
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &Vector2::x)
		.property("y", &Vector2::y);
		);

	MRK_CORE_REFLECT_REGISTER(Vector3)(
		rttr::registration::class_<Vector3>("Vector3")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &Vector3::x)
		.property("y", &Vector3::y)
		.property("z", &Vector3::z);
		);

	MRK_CORE_REFLECT_REGISTER(Vector4)(
		rttr::registration::class_<Vector4>("Vector4")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &Vector4::x)
		.property("y", &Vector4::y)
		.property("z", &Vector4::z)
		.property("w", &Vector4::w);
		);

	MRK_CORE_REFLECT_REGISTER(Vector2i)(
		rttr::registration::class_<Vector2i>("Vector2i")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &Vector2i::x)
		.property("y", &Vector2i::y);
		);

	MRK_CORE_REFLECT_REGISTER(Vector3i)(
		rttr::registration::class_<Vector3i>("Vector3i")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &Vector3i::x)
		.property("y", &Vector3i::y)
		.property("z", &Vector3i::z);
		);

	MRK_CORE_REFLECT_REGISTER(Vector4i)(
		rttr::registration::class_<Vector4i>("Vector4i")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &Vector4i::x)
		.property("y", &Vector4i::y)
		.property("z", &Vector4i::z)
		.property("w", &Vector4i::w);
		);

	MRK_CORE_REFLECT_REGISTER(Quaternion)(
		rttr::registration::class_<Quaternion>("Quaternion")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &Quaternion::x)
		.property("y", &Quaternion::y)
		.property("z", &Quaternion::z)
		.property("w", &Quaternion::w);
		);

	MRK_CORE_REFLECT_REGISTER(Matrix4)(
		rttr::registration::class_<Matrix4>("Matrix4")
		.constructor<>()(rttr::policy::ctor::as_object)
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
	MRK_CORE_REFLECT_REGISTER(Object)(
		rttr::registration::class_<Object>("Object")
		.constructor<>();
		);

	MRK_CORE_REFLECT_REGISTER(GameObject)(
		rttr::registration::class_<GameObject>("GameObject")
		.property("id", &GameObject::GetID, &GameObject::SetID_)
		.property("name", &GameObject::GetName, &GameObject::SetName);
		);

	MRK_CORE_REFLECT_REGISTER(Component)(
		rttr::registration::class_<Component>("Component")
		.constructor<>();
		);
}

#include "Core/Application/Application.h"

namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(AppConfig)(
		rttr::registration::class_<AppConfig>("AppConfig")
		.property("size", &AppConfig::size)
		.property("title", &AppConfig::title);
		);
}
