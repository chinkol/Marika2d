#include <iostream>

#include "Core/Application/Application.h"
#include "Core/OpenGL/OpenGL.h"
#include "Core/Mesh/Mesh.h"

#include "Editor/Asset/AssetSystem.h"
#include "Editor/Plugin/Plugin.h"

#include "Third/SOIL2/SOIL2.h"
#include "Third/imgui/imgui.h"
#include "Third/imgui/imgui_browser.h"

#include "RttrTest.h"

#include <string>

//void RttrTestFunc()
//{
//	std::string json_string;
//
//	{
//		circle c_1("Circle #1");
//		shape& my_shape = c_1;
//
//		c_1.set_visible(true);
//		c_1.points = std::vector<point2d>(2, point2d(1, 1));
//		c_1.points[1].x = 23;
//		c_1.points[1].y = 42;
//
//		c_1.position.x = 12;
//		c_1.position.y = 66;
//
//		c_1.radius = 5.123;
//		c_1.color_ = color::red;
//
//		// additional braces are needed for a VS 2013 bug
//		c_1.dictionary = { { {color::green, {1, 2} }, {color::blue, {3, 4} }, {color::red, {5, 6} } } };
//
//		c_1.no_serialize = 12345;
//
//		MrkNew::ReflectSystem::ToJson(c_1);
//	}
//}

void Test()
{
	auto mesh = Mrk::MeshHouse::GetMesh("D:/SourceCode/Marika2d/Marika2d/TestProject/test.mem");
}

void EditorLoopTest()
{
	static auto tex1 = SOIL_load_OGL_single_cubemap("OpenGL-DDS/Textures/desertcube1024.cbdds", SOIL_DDS_CUBEMAP_FACE_ORDER, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);
	static auto tex2 = SOIL_load_OGL_texture("OpenGL-DDS/Textures/grass.dds", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);
	
	ImGui::DockSpaceOverViewport();

	//viewport
	ImGui::Begin("main viewport");
	ImGui::Image((ImTextureID)tex1, ImGui::GetWindowSize());
	ImGui::End();

	ImGui::Begin("edit viewport");
	ImGui::Image((ImTextureID)tex2, ImGui::GetWindowSize());
	ImGui::End();

	ImGui::Begin("debug logger");
	ImGui::End();

	ImGui::Begin("component watcher");
	ImGui::End();

	ImGui::Begin("project manager");
	ImGui::End();

	ImGui::Begin("scene tree");
	ImGui::End();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Create"))
			{
				Mrk::PluginCreateProject::GetInstance()->SelectFile();
			}
			if (ImGui::MenuItem("Open"))
			{
			}
			if (ImGui::MenuItem("Save"))
			{
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
			{
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo"))
			{
			}
			if (ImGui::MenuItem("Redo"))
			{
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Assert"))
		{
			if (ImGui::MenuItem("Import"))
			{
				Mrk::PluginAssetImport::GetInstance()->SelectFiles();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("New"))
		{
			if (ImGui::BeginMenu("GameObject"))
			{
				auto& manifest = Mrk::GameObjectFactory::GetManifest();
				for (auto& item : manifest)
				{
					if (ImGui::MenuItem(item.c_str()))
					{
						auto obj = Mrk::GameObjectFactory::CreateNew(item);
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
void ModelImprotTest()
{
	
}

int main()
{
	RttrTestFunc();

	auto context = Mrk::Application::GetAppContext();

	context.windowTitle = "Marika Engine Editor";
	context.windowSize = { 1280, 800 };

	context.appInitedCallBack = []() {
		Mrk::PluginSystem::Init();
		};

	context.updateCallBack = []() {

		{

			EditorLoopTest();
			Test();

		}
		
		Mrk::PluginSystem::Update();

		{

		}

		};

	context.drawCallBack = []() {
		Mrk::PluginSystem::Draw();
		};

	Mrk::Application::SetAppContext(context);
	Mrk::Application::Run();
}
