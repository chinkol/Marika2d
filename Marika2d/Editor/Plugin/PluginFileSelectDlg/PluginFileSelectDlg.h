#pragma once

#include "Editor/Plugin/Plugin.h"

namespace Mrk::Editor
{
	class PluginFileSelectDlg : public IPlugin
	{
		MRK_PLUGIN(PluginFileSelectDlg)
	public:
		void Update();
	private:
		std::map<std::string, ImGui::FileBrowser> pathSelectors;
	};
}


