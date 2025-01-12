#pragma once

#include "Common/Singleton/Singleton.h"
#include "Common/Def/UtilityDef.h"

#include "Core/Config/ConfigSys.h"

#include "Third/imgui/imgui_browser.h"

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <map>

#ifndef MRK_PLUGIN
#define MRK_PLUGIN(x)												\
friend class PluginSystem;											\
public:																\
static inline x* GetInstance() {									\
	return instance;												\
}																	\
private:															\
inline x() {														\
	x::instance = this;												\
}																	\
virtual inline std::string_view GetPluginName() override {			\
	return #x;														\
}																	\
static inline bool _mrk_macro_##x##_editor_plugin_register_ = [](){	\
	Mrk::PluginSystem::RegisterPlugin<x>();							\
	return true;													\
}();																\
static inline x* instance = nullptr;
#endif // !MRK_PLUGIN

namespace Mrk
{
	class IPlugin;

	class PluginSystem : public Singleton<PluginSystem>
	{
		MRK_SINGLETON(PluginSystem)
	public:
		template<typename T> static void RegisterPlugin();
		static void Init();
		static void Update();
		static void Draw();
	private:
		std::vector<std::function<IPlugin* ()>> creators;
		std::vector<IPlugin*> plugins;
	};

	class IPlugin
	{
	public:
		virtual ~IPlugin() = default;
		virtual inline void Init() {};
		virtual inline void Update() {};
		virtual inline void Draw() {};
		virtual std::string_view GetPluginName() = 0;
	};

	class PluginAssetImport : public IPlugin
	{
		MRK_PLUGIN(PluginAssetImport)
	public:
		void SelectFiles();

	private:
		virtual void Init() override;
		virtual void Update();
		
	private:
		ImGui::FileBrowser fileDlg;
		ImGui::FileBrowser saveDlg;
		std::vector<std::filesystem::path> fromPathes;
		std::filesystem::path toPath;
	};

	class PluginCreateProject : public IPlugin
	{
		MRK_PLUGIN(PluginCreateProject)
	public:
		void SelectFile();
	private:
		void CreateNewProj(const std::filesystem::path& path);
		virtual void Init() override;
		virtual void Update() override;
	private:
		ImGui::FileBrowser pathSelectDlg;
		std::filesystem::path projectPath;
		std::string empty = std::string("", 64);
		std::string projectName = empty;
	};

	// ±à¼­Æ÷ÅäÖÃ
	class PluginEdtorConfig : public IPlugin
	{
		MRK_PLUGIN(PluginEdtorConfig)
	public:
		void Show()
		{
			needShow = true;
		}

		virtual inline void Draw() override
		{
			if (needShow)
			{
				ImGui::Begin("Editor Setting");

				auto groups = ConfigSys::GetConfigGroups();
				for (auto& [name, group] : groups)
				{
					for (auto iter = group.romote.MemberBegin(); iter != group.romote.MemberEnd(); iter++)
					{
						auto key = iter->name.GetString();
						auto& val = iter->value;

						if (val.IsBool())
						{
							bool res;
							ImGui::Checkbox(key, &res);
						}
					}
				}

				ImGui::End();
			}
		}
	private:
		bool needShow = false;
	};
}

namespace Mrk
{
	template<typename T>
	inline void PluginSystem::RegisterPlugin()
	{
		static_assert(std::is_base_of_v<IPlugin, T>, "T Is Not A Plugin");
		Instance().creators.push_back([]() {
			return new T();
			});
	}
}


