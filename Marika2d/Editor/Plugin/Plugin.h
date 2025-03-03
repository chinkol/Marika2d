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
friend class PluginSys;												\
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
	Mrk::PluginSys::RegisterPlugin<x>();							\
	return true;													\
}();																\
static inline x* instance = nullptr;
#endif // !MRK_PLUGIN

namespace Mrk
{
	class IPlugin;
	class GameObject;

	class PluginSys : public Singleton<PluginSys>
	{
		MRK_SINGLETON(PluginSys)
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

	class PluginProjectCreater : public IPlugin
	{
		MRK_PLUGIN(PluginProjectCreater)
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

	class PluginObjectSelecter : public IPlugin
	{
		MRK_PLUGIN(PluginObjectSelecter)
	public:
		std::shared_ptr<GameObject> GetSelection();
		void SetSelection(std::shared_ptr<GameObject> selection);
		void PickSelectionFromViewport() {};
	private:
		std::weak_ptr<GameObject> selection;
	};

	class PluginSceneLoader : public IPlugin
	{
		MRK_PLUGIN(PluginSceneLoader)
	public:
		void SelectFile();
	private:
		virtual void Init() override;
		virtual void Update() override;
	private:
		ImGui::FileBrowser pathSelectDlg;
	};

	class PluginSceneTreeUI : public IPlugin
	{
		MRK_PLUGIN(PluginSceneTreeUI)
	public:
		virtual void Draw() override;
	private:
		void CreateTreeNode(std::shared_ptr<GameObject> node);
	};

	class PluginViewportUI : public IPlugin
	{
		MRK_PLUGIN(PluginViewportUI)
	public:
		virtual void Draw() override;
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

				/*auto groups = ConfigSys::GetConfigGroups();
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
				}*/

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
	inline void PluginSys::RegisterPlugin()
	{
		static_assert(std::is_base_of_v<IPlugin, T>, "T Is Not A Plugin");
		Instance().creators.push_back([]() {
			return new T();
			});
	}
}