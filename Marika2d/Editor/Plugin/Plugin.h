#pragma once

#include "Common/Singleton/Singleton.h"

#include <string>
#include <functional>
#include <vector>

#ifndef MRK_PLUGIN
#define MRK_PLUGIN(x)												\
virtual inline std::string_view GetPluginName() override {			\
	return #x; }													\
static inline bool _mrk_##x##_editor_plugin_register_ = [](){		\
	Mrk::PluginSystem::RegisterPlugin<x>(); return true;}();
#endif // !MRK_PLUGIN

namespace Mrk
{
	class IPlugin;

	class PluginSystem : public Singleton<PluginSystem>
	{
		MRK_SINGLETON(PluginSystem)
	public:
		template<typename T>
		static void RegisterPlugin() 
		{
			static_assert(std::is_base_of_v<IPlugin, T>, "T Is Not A Plugin");
			Instance().creators.push_back([]() {
				return new T();
				});
		}
		static void Init();
		static void Update();
		static void Draw();
	private:
		std::vector<std::function<IPlugin*()>> creators;
		std::vector<IPlugin*> plugins;
	};

	class IPlugin
	{
	public:
		virtual inline void Update() {};
		virtual inline void Draw() {};
		virtual std::string_view GetPluginName() = 0;
	};

	class FileDlgBackEnd : public IPlugin
	{
		MRK_PLUGIN(FileDlgBackEnd)
	public:

	};
}


