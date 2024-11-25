#include "Plugin.h"

#include "Editor/Asset/AssetSystem.h"

void Mrk::PluginSystem::Init()
{
	Instance().plugins.clear();
	for (auto& creator : Instance().creators)
	{
		auto plugin = creator();
		plugin->Init();
		Instance().plugins.push_back(plugin);
	}
}

void Mrk::PluginSystem::Update()
{
	for (auto& plugin : Instance().plugins)
	{
		plugin->Update();
	}
}

void Mrk::PluginSystem::Draw()
{
	for (auto& plugin : Instance().plugins)
	{
		plugin->Draw();
	}
}

void Mrk::PluginAssetImport::SelectFiles()
{
	fileDlg.Open();
}

void Mrk::PluginAssetImport::Init()
{
	fileDlg = ImGui::FileBrowser(ImGuiFileBrowserFlags_MultipleSelection | ImGuiFileBrowserFlags_ConfirmOnEnter | ImGuiFileBrowserFlags_CloseOnEsc);
	fileDlg.SetTitle("Import Asset");
	fileDlg.SetTypeFilters(AssetSystem::GetImportSupports());

	saveDlg = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_HideRegularFiles | ImGuiFileBrowserFlags_CreateNewDir);
	saveDlg.SetTitle("Save Asset To");
}

void Mrk::PluginAssetImport::Update()
{
	fileDlg.Display();
	if (fileDlg.HasSelected())
	{
		fromPathes = fileDlg.GetMultiSelected();
		fileDlg.ClearSelected();
		saveDlg.Open();
	}

	saveDlg.Display();
	if (saveDlg.HasSelected())
	{
		toPath = saveDlg.GetDirectory();
		saveDlg.ClearSelected();
	}

	if (!fromPathes.empty() && !toPath.empty())
	{
		for (auto& fromPath : fromPathes)
		{
			AssetSystem::Import(fromPath, toPath);
		}
		fromPathes.clear();
		toPath.clear();
	}
}

void Mrk::PluginCreateProject::SelectFile()
{
	pathSelectDlg.Open();
}

void Mrk::PluginCreateProject::CreateNewProj(const std::filesystem::path& path)
{
	std::filesystem::create_directory(path);
	//TODO
}

void Mrk::PluginCreateProject::Init()
{
	pathSelectDlg = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
	pathSelectDlg.SetTitle("Create Project");
}

void Mrk::PluginCreateProject::Update()
{
	pathSelectDlg.Display();
	if (pathSelectDlg.HasSelected())
	{
		projectPath = pathSelectDlg.GetSelected();
		pathSelectDlg.ClearSelected();

		ImGui::OpenPopup((char*)u8"工程名称");
	}

	if (ImGui::BeginPopupModal((char*)u8"工程名称"))
	{
		ImGui::InputText((char*)u8"工程名称", projectName.data(), projectName.size());
		if (ImGui::Button("OK") && projectName != empty)
		{
			CreateNewProj(projectPath / projectName);
			projectName = empty;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
