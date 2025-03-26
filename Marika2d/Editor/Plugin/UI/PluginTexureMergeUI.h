#pragma once

#include "Editor/Plugin/Plugin.h"

namespace Mrk
{
	class Mesh;

	class PluginTexureMergeUI : public IPlugin
	{
		MRK_PLUGIN(PluginTexureMergeUI)
	public:
		void Draw();
	private:
		float canvaScale = 1.0f;
		std::shared_ptr<Mesh> mesh;
	};
}


