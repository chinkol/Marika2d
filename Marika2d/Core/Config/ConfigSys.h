#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/Reflect/Reflect.h"
#include "Core/Object/Object.h"

#include <map>
#include <string>
#include <functional>

#ifndef MRK_CONFIG
#define MRK_CONFIG(x)																								\
RTTR_ENABLE(ConfigGroup)																							\
friend class ConfigSys;																								\
private:																											\
static inline bool _mrk_mrcro_config_group_##x##_register_ = [](){													\
	Mrk::ConfigSys::RegisterConfigGroup<x>(#x);																		\
	return true;																									\
	}();																											\
x() = default;																										\
public:																												\
virtual inline rttr::variant GetConfigItemVariant(std::string_view name) override									\
{																													\
	auto prop = rttr::type::get<decltype(*this)>().get_property(name.data());										\
	if (prop.is_valid())																							\
	{																												\
		return prop.get_value(*this);																				\
	}																												\
	return rttr::variant();																							\
}
#endif // !MRK_CONFIG

#ifndef MRK_CONFIG_PATH 
#define MRK_CONFIG_PATH "Config"
#endif // !MRK_CONFIG_PATH 

#ifndef MRK_CONFIG_EXTENSION 
#define MRK_CONFIG_EXTENSION ".mcfg"
#endif // !MRK_CONFIG_EXTENSION 

namespace Mrk
{
	class ConfigGroup : public Object
	{
		MRK_OBJECT(ConfigGroup) RTTR_ENABLE(Object) MRK_OBJECT_SERIALIZE_SELF(ConfigGroup)
	public:
		virtual rttr::variant GetConfigItemVariant(std::string_view name) = 0;
	};

	class ConfigSys : Singleton<ConfigSys>
	{
		MRK_SINGLETON(ConfigSys)
	public:
		template<typename T> 
		static T GetConfigItem(std::string_view group, std::string_view name);
		static rttr::variant GetConfigItemVariant(std::string_view group, std::string_view name);
		template<typename T>
		static void RegisterConfigGroup(std::string_view name);
		static void Init();
	private:
		std::map<std::string, ConfigGroup*> groups;
		std::map<std::string, ConfigGroup*> defaults;
		std::map<std::string, std::function<ConfigGroup* ()>> ctors;
	};
}

namespace Mrk
{
	template<typename T>
	inline T ConfigSys::GetConfigItem(std::string_view group, std::string_view name)
	{
		auto ret = Instance().groups.find(group.data());
		if (ret != Instance().groups.end())
		{
			auto variant = ret->second->GetConfigItemVariant(name);
			if (variant.convert(rttr::type::get<T>()))
			{
				return variant.get_value<T>();
			}
			else
			{
				//TODO : throw
			}
		}

		return T();	//TODO : throw
	}

	template<typename T>
	inline void ConfigSys::RegisterConfigGroup(std::string_view name)
	{
		static_assert(std::is_base_of_v<ConfigGroup, T>, "T is not ConfigGroup !");

		Instance().ctors.try_emplace(name.data(), []() {
			return new T();
			});
	}
}