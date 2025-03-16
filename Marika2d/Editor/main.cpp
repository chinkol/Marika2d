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
