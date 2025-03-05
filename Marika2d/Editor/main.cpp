#include <iostream>

#include "GenEditorReflectInfo/GenEditorReflectInfo.h"

#include "Core/GenCoreReflectInfo/GenCoreReflectInfo.h"
#include "Core/Reflect/Reflect.h"
#include "Core/Config/ConfigSys.h"
#include "Core/Application/Application.h"
#include "Core/OpenGL/OpenGL.h"
#include "Core/Mesh/Mesh.h"

#include "Editor/Asset/AssetSys.h"
#include "Editor/Plugin/Plugin.h"

#include "Third/SOIL2/SOIL2.h"
#include "Third/imgui/imgui.h"
#include "Third/imgui/imgui_browser.h"
#include "Third/imgui/freetype/imgui_freetype.h"

#include "RttrTest.h"

#include <string>

void AssetTest()
{

	//Mrk::AssetSys::Import("../TestProject/Mualani/Mualani.fbx", Mrk::ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
}

void MeshTest()
{
	auto mesh = Mrk::MeshHut::GetMesh("D:/QinKL/SorceCode/Marika2d/koko/Marika2d/Marika2d/TestProject/FuNingNa/Meshes/Body.mmsh");
	int i = 0;
}

void EditorLoopTest()
{
	static auto tex1 = SOIL_load_OGL_single_cubemap("OpenGL-DDS/Textures/desertcube1024.cbdds", SOIL_DDS_CUBEMAP_FACE_ORDER, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);
	static auto tex2 = SOIL_load_OGL_texture("OpenGL-DDS/Textures/grass.dds", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);

	ImGui::DockSpaceOverViewport();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Create"))
			{
				Mrk::PluginProjectCreater::GetInstance()->SelectFile();
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

		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::MenuItem("Load(.msce)"))
			{
				Mrk::PluginSceneLoader::GetInstance()->SelectFile();
			}

			if (ImGui::MenuItem("Save(.msce)"))
			{
				Mrk::PluginSceneSaver::GetInstance()->SelectFile();
			}

			if (ImGui::MenuItem("New"))
			{
				Mrk::SceneHut::CreateNew("Scene", 10000);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Load(.mpre)"))
			{

			}

			if (ImGui::BeginMenu("GameObject"))
			{
				auto& manifest = Mrk::GameObjectFactory::GetManifest();
				for (auto& item : manifest)
				{
					if (ImGui::MenuItem(item.c_str()))
					{
						auto obj = Mrk::GameObjectFactory::CreateNew(item);
						if (auto selection = Mrk::PluginObjectSelecter::GetInstance()->GetSelection())
						{
							selection->AddChild(obj);
						}
						else if (auto scene = Mrk::SceneHut::GetCurrScene())
						{
							scene->GetRoot()->AddChild(obj);
						}
					}
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Test"))
		{
			if (ImGui::MenuItem("ReLoad Font"))
			{
			}

			if (ImGui::MenuItem("ReLoad Font With FreeType"))
			{
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
void ModelImprotTest()
{

}
void ConfigSysTest()
{

}
int main()
{
	// App Init
	{
		Mrk::GenCoreReflectInfo();
		Mrk::GenEditorReflectInfo();

		Mrk::ConfigSys::Init();
	}

	auto context = Mrk::Application::GetAppContext();
	context.appInitedCallBack = []() {
		Mrk::PluginSys::Init();

		std::shared_ptr<Mrk::GameObject> recive;
		MrkTest::Test(recive);

		AssetTest();
		MeshTest();
		};

	context.updateCallBack = []() {

		{

			EditorLoopTest();


		}

		Mrk::PluginSys::Update();

		{

		}

		};

	context.drawCallBack = []() {
		Mrk::PluginSys::Draw();
		};

	Mrk::Application::SetAppContext(context);
	Mrk::Application::Run();
}