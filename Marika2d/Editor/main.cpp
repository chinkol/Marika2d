#include <iostream>

#include "Core/Application/Application.h"
#include "Core/OpenGL/OpenGL.h"

#include "Editor/Resource/ResourceImporter.h"
#include "Editor/Plugin/Plugin.h"

#include "Third/SOIL2/SOIL2.h"
#include "Third/ImGuiFileDialog/ImGuiFileDialog.h"

void EditorLoop()
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
				IGFD::FileDialogConfig config;
				config.path = ".";
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp", config);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::cout << filePathName << "\t" << filePath << "\n";
		}
		ImGuiFileDialog::Instance()->Close();
	}
}
void ImguiFileDialogTest()
{

}
void ModelImprotTest()
{
	Mrk::IModelImporter* fbxImporter = new Mrk::FbxImporter();
	auto i = fbxImporter->GetExtension();
	//fbxImporter->Import("..");
}

int main()
{
	Mrk::PluginSystem::Init();

	auto context = Mrk::Application::GetAppContext();
	context.mainwndTitle = "Marika Engine Editor";
	context.mainwndBox = { 1280, 800 };
	context.loopCallBacks.push_back([]() {
		EditorLoop();
		//ModelImprotTest();
		//ImguiFileDialogTest();
		Mrk::PluginSystem::Update();
		Mrk::PluginSystem::Draw();
		});
	Mrk::Application::SetAppContext(context);
	Mrk::Application::Run();	
}
