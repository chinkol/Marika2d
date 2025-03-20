#pragma once

#include "Editor/Plugin/Plugin.h"

namespace Mrk::Editor
{
	class PluginPathSelectDlg : public IPlugin
	{
		MRK_PLUGIN(PluginPathSelectDlg)
	public:
		void Init();
		void Update();
		void Draw();

		void SelectDir(std::function<void(const std::filesystem::path&)> callback);
		void SelectFile(std::function<void(const std::filesystem::path&)> callback);
		void SelectFiles(std::function<void(const std::vector<std::filesystem::path>&)> callback);
		void SelectNewFile(std::function<void(const std::filesystem::path&)> callback);

	private:
		ImGui::FileBrowser dirSelector;
		std::function<void(const std::filesystem::path&)> dirCallback;

		ImGui::FileBrowser fileSelector;
		std::function<void(const std::filesystem::path&)> fileCallback;

		ImGui::FileBrowser filesSelector;
		std::function<void(const std::vector<std::filesystem::path>&)> filesCallback;

		ImGui::FileBrowser newFileSelector;
		std::function<void(const std::filesystem::path&)> newFileCallback;
	};
}
