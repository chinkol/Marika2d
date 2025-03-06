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

		core_reflect_registers.clear();
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
	CoreTypeReflectRegister();
}

#ifndef MRK_CORE_REFLECT_REGISTER_CONTENT
#define MRK_CORE_REFLECT_REGISTER_CONTENT(x) x }); return true; }();
#endif // !MRK_CORE_REFLECT_REGISTER_CONTENT

#ifndef MRK_CORE_REFLECT_REGISTER
#define MRK_CORE_REFLECT_REGISTER(x) static inline bool _mrk_rttr_core_type_##x##_reflect_register_ = [](){core_reflect_registers.push_back([](){ MRK_CORE_REFLECT_REGISTER_CONTENT
#endif // !MRK_CORE_REFLECT_REGISTER

#include "Core/IDGenerater/IDGenerater.h"

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
		.constructor<>()(rttr::policy::ctor::as_object);
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
}

#include "Core/Component/Component.h"


namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(Component)(
		rttr::registration::class_<Component>("Component");
		);
}

#include "Core/Component/Transform/Transform.h"

namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(Transform)(
		rttr::registration::class_<Transform>("Transform")
		.property("localPosition", &Transform::GetLocalPosition, &Transform::SetLocalPosition)
		.property("localRotation", &Transform::GetLocalRotation, &Transform::SetLocalRotation)
		.property("localScale", &Transform::GetLocalScale, &Transform::SetLocalScale);
		)
}


#include "Core/Mesh/MeshRenderer.h"

namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(MeshRenderer)(
		rttr::registration::class_<MeshRenderer>("MeshRenderer")
		.property("meshPath", &MeshRenderer::GetMeshPath, &MeshRenderer::SetMeshPath)
		.property("vsPath", &MeshRenderer::GetVsPath, &MeshRenderer::SetVsPath)
		.property("fsPath", &MeshRenderer::GetFsPath, &MeshRenderer::SetFsPath);
		);
}

#include "Core/Application/Application.h"

namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(AppConfig)(
		rttr::registration::class_<AppConfig>("AppConfig")
		.property("size", &AppConfig::size)
		.property("title", &AppConfig::title)
		.property("projDir", &AppConfig::projDir)
		.property("fontPath", &AppConfig::fontPath);
		);
}

#include "Core/Mesh/Mesh.h"

namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(Vertex)(
		rttr::registration::class_<Vertex>("Vertex")
		.property("position", &Vertex::position)
		.property("normal", &Vertex::normal)
		.property("texcoord", &Vertex::texcoord);
		);

	MRK_CORE_REFLECT_REGISTER(SubMesh)(
		rttr::registration::class_<SubMesh>("SubMesh")
		.property("offset", &SubMesh::offset)
		.property("count", &SubMesh::count);
		);
}

#include "Core/Camera/Camera.h"

namespace Mrk
{
	MRK_CORE_REFLECT_REGISTER(Camera)(
		rttr::registration::class_<Camera>("Camera");
		);

	MRK_CORE_REFLECT_REGISTER(CameraOutput)(
		rttr::registration::class_<CameraOutput>("CameraOutput")
		.property("resolution", &CameraOutput::GetResolution, &CameraOutput::SetResolution)
		.property("frumstum", &CameraOutput::GetFrustum, &CameraOutput::SetFrustum_);
		);

	MRK_CORE_REFLECT_REGISTER(CameraFrustum)(
		rttr::registration::class_<CameraFrustum>("CameraFrustum")
		.property("nearZ", &CameraFrustum::GetNearZ, &CameraFrustum::SetNearZ)
		.property("farZ", &CameraFrustum::GetFarZ, &CameraFrustum::SetFarZ)
		.property("aspect", &CameraFrustum::GetAspect, &CameraFrustum::SetAspect);
		);
}
