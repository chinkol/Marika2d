#include "GenEditorReflectInfo.h"

#include <vector>
#include <functional>

namespace
{
	std::vector<std::function<void()>> editor_reflect_registers;
}

void Mrk::GenEditorReflectInfo()
{
	for (auto& efitor_reflect_register : editor_reflect_registers)
	{
		if (efitor_reflect_register)
		{
			efitor_reflect_register();
		};
	}
}

#ifndef MRK_EDITOR_REFLECT_REGISTER_CONTENT
#define MRK_EDITOR_REFLECT_REGISTER_CONTENT(x) x }); return true; }();
#endif // !MRK_EDITOR_REFLECT_REGISTER_CONTENT

#ifndef MRK_EDITOR_REFLECT_REGISTER
#define MRK_EDITOR_REFLECT_REGISTER(x) static inline bool _mrk_rttr_editor_type_##x##_reflect_register_ = [](){editor_reflect_registers.push_back([](){ MRK_EDITOR_REFLECT_REGISTER_CONTENT
#endif // !MRK_EDITOR_REFLECT_REGISTER

#include "Editor/RttrTest.h"

namespace MrkTest
{
	MRK_EDITOR_REFLECT_REGISTER(BoneComponent)(
		rttr::registration::class_<BoneComponent>("BoneComponent")
		.constructor<>()
		.property("data0", &BoneComponent::data0)
		.property("data1", &BoneComponent::GetData1, &BoneComponent::SetData1)
		.property("data2", &BoneComponent::data2)
		.property("data3", &BoneComponent::data3)
		.property("data4", &BoneComponent::data4);
		);

	MRK_EDITOR_REFLECT_REGISTER(Bone)(
		rttr::registration::class_<Bone>("Bone")
		.constructor<>();
		);
}
