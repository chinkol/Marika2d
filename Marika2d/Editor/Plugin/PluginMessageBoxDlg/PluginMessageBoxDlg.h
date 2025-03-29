#pragma once

#include "Editor/Plugin/Plugin.h"

namespace Mrk
{
	class PluginMessageBoxDlg : public IPlugin
	{
		MRK_PLUGIN(PluginMessageBoxDlg)
	public:
		void Pop(std::string_view message);
		void Draw();
	private:
		std::set<std::string> messages;
	};
}


