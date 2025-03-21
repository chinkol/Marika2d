#include "PluginPathSelectDlg.h"

void Mrk::Editor::PluginPathSelectDlg::SelectDir(std::function<void(const std::filesystem::path&)> callback)
{
	dirCallback = callback;
	dirSelector.Open();
}

void Mrk::Editor::PluginPathSelectDlg::SelectFile(std::function<void(const std::filesystem::path&)> callback)
{
	fileCallback = callback;
	fileSelector.Open();
}

void Mrk::Editor::PluginPathSelectDlg::SelectFiles(std::function<void(const std::vector<std::filesystem::path>&)> callback)
{
	filesCallback = callback;
	filesSelector.Open();
}

void Mrk::Editor::PluginPathSelectDlg::SelectNewFile(std::function<void(const std::filesystem::path&)> callback)
{
	newFileCallback = callback;
	newFileSelector.Open();
}

void Mrk::Editor::PluginPathSelectDlg::Update()
{
	auto ProcessSelection = [](auto& selector, auto& callback) {
		if (callback && selector.HasSelected())
		{
			callback(selector.GetSelected());
			callback = nullptr;
			selector.ClearSelected();
		}
		};

	ProcessSelection(fileSelector, fileCallback);
	ProcessSelection(newFileSelector, newFileCallback);
	ProcessSelection(dirSelector, dirCallback);

	if (filesCallback && filesSelector.HasSelected())
	{
		filesCallback(filesSelector.GetMultiSelected());
		filesCallback = nullptr;
		filesSelector.ClearSelected();
	}
}

void Mrk::Editor::PluginPathSelectDlg::Draw()
{
	dirSelector.Display();
	fileSelector.Display();
	filesSelector.Display();
	newFileSelector.Display();
}

void Mrk::Editor::PluginPathSelectDlg::Init()
{
	newFileSelector = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_ConfirmOnEnter | ImGuiFileBrowserFlags_CreateNewDir | ImGuiFileBrowserFlags_EnterNewFilename, ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
	newFileSelector.SetTitle("Select New File");

	fileSelector = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_ConfirmOnEnter, ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
	fileSelector.SetTitle("Select File");

	filesSelector = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_ConfirmOnEnter | ImGuiFileBrowserFlags_MultipleSelection, ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
	filesSelector.SetTitle("Select Files");

	dirSelector = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_ConfirmOnEnter | ImGuiFileBrowserFlags_SelectDirectory, ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
	dirSelector.SetTitle("Select Directory");
}
