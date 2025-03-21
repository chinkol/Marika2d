#pragma once

#include "Editor/Plugin/Plugin.h"

namespace Mrk
{
	class PluginInputDlg : public IPlugin
	{
		MRK_PLUGIN(PluginInputDlg)
	public:
		virtual void Draw();

		void GetInt(std::function<void(int)> callback);
		void GetFloat(std::function<void(float)> callback);
		void GetString(std::function<void(const std::string&)> callback);

	private:
		std::function<void(int)> intCallback = nullptr;
		std::function<void(float)> floatCallback = nullptr;
		std::function<void(const std::string&)> stringCallback = nullptr;
	};
}