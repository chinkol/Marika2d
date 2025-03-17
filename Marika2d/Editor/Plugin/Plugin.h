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
	class Component;

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

	class PluginSceneSaver : public IPlugin
	{
		MRK_PLUGIN(PluginSceneSaver)
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
		void DragDropNode(std::shared_ptr<GameObject> node);
	};

	class PluginViewportUI : public IPlugin
	{
		MRK_PLUGIN(PluginViewportUI)
	public:
		virtual void Draw() override;
	};

	// 编辑器配置
	class PluginEdtorConfigUI : public IPlugin
	{
		MRK_PLUGIN(PluginEdtorConfigUI)
	public:
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

	class PluginPropertiesInspectUI : public IPlugin
	{
		MRK_PLUGIN(PluginPropertiesInspectUI)
	public:
		void Draw();
	private:
		bool RecurProperties(rttr::instance obj);
		bool RecurObject(rttr::instance obj, std::string_view name);
		bool RecurArithmetic(rttr::variant& base, std::string_view name);
		bool RecurSeqContainer(rttr::variant& array, std::string_view name);
		bool RecurAssContainer(rttr::variant& array, std::string_view name);
		bool RecurString(rttr::variant& str, std::string_view name);
		bool RecurVariant(rttr::variant& variant, std::string_view name);
		bool RecurSpecialized(rttr::variant& variant, std::string_view name);	// 处理特化类型
		bool IsSepcializedType(const rttr::type& type);
	};

	class PluginImNodesTest : public IPlugin
	{
		MRK_PLUGIN(PluginImNodesTest)
	public:
		void Draw();
	};

	class PluginMaterialEditor : public IPlugin
	{
		MRK_PLUGIN(PluginMaterialEditor)
	public:
		void SetSelectedMaterialFile(std::string_view file);
		void Draw();
	private:
		std::string selectedMaterialFile;
	};
}

namespace Mrk
{
	template<typename T>
	inline void PluginSys::RegisterPlugin()
	{
		MRK_INSTANCE_REF;

		static_assert(std::is_base_of_v<IPlugin, T>, "T Is Not A Plugin");
		instance.creators.push_back([]() {
			return new T();
			});
	}
}