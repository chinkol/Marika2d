#pragma execution_character_set("utf-8")

#include <iostream>
#include <sstream>
#include <map>

#include "Common/Log/LogSystem.h"
#include "Common/Serialize/SerializeSystem.h"
#include "Common/Memory/MemCtrlSystem.h"
#include "Common/Def/UtilityDef.h"

#include "Core/Application/Application.h"
#include "Core/GameObject/GameObject.h"
#include "Core/Component/Component.h"
#include "Core/Scene/Scene.h"

#include "Third/rapidjson/document.h"
#include "Third/rapidjson/writer.h"
#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/stringbuffer.h"
#include "Third/box2d/box2d.h"
#include "Third/glad/include/glad.h"
#include "Third/glfw/include/glfw3.h"

class TestComponent : public Mrk::Component
{
	MRK_COMPONENT(TestComponent)
private:
	void Awake()
	{
		std::cout << "Awake\n";
	}
	void Start()
	{
		std::cout << "Start\n";
	}
	void PreUpdate()
	{
		std::cout << "PreUpdate\n";
	}
	void Update()
	{
		std::cout << "Update\n";
	}
	void FixedUpdate()
	{
		std::cout << "FixedUpdate\n";
	}
	void Draw()
	{
		std::cout << "Draw\n";
	}
	void LateDraw()
	{
		std::cout << "LateDraw\n";
	}
};

int main()
{
	Mrk::OctTree octTree(1000, 1000, 1000);

	auto com1 = Mrk::ComponentFactory::CreateNew("TestComponent");

	auto context = Mrk::Application::GetAppContext();
	context.updateCallBack = []() {
		ImGui::ShowDebugLogWindow();
		};
	Mrk::Application::SetAppContext(context);
	Mrk::Application::Run();

	return 0;
}