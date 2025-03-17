#pragma once

#include "Editor/Plugin/Plugin.h"

namespace Mrk
{
	class PluginMenuBarUI : public IPlugin
	{
		MRK_PLUGIN(PluginMenuBarUI)
	public:
		void Draw();
	};

}

