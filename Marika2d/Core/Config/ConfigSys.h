#pragma once

#include <string>
#include <map>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "Common/Singleton/Singleton.h"

#include "Core/Global/Global.h"

#include "Third/rapidjson/document.h"
#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/stringbuffer.h"
#include "Third/sigslot/signal.hpp"

// tips : ������ר�õĵط�(�������ô���)����Set/GetAndSet�������ط�����ֻ����Get/GetOrSet

namespace Mrk
{
	class ConfigSys : Mrk::Singleton<ConfigSys>
	{
		MRK_SINGLETON(ConfigSys)
	private:
		struct ConfigGroup
		{
			Json::Document romote;
			Json::Document local;
			bool isDirty = false;
			ConfigGroup();
		};

	public:
		static const std::map<std::string, ConfigGroup>& GetConfigGroups();

		template<typename T>	// �������򷵻�����ֵ�����򷵻�Ĭ��ֵ
		static T Get(std::string_view groupName, std::string_view configName, T defaultVal = T());

		template<typename T>	// ����ֵΪval
		static void Set(std::string_view groupName, std::string_view configName, T val);
		
		template<typename T>	// �������򷵻�����ֵ, ��������ֵΪdefaultVal������, (���δ򿪳���������ļ��쳣ʱ, �ᴥ����ʱ�����򱣴�, ���ܻ����һ�����������)
		static T GetOrSet(std::string_view groupName, std::string_view configName, T defaultVal = T());

		template<typename T>	// ����ֵΪval������
		static T GetAndSet(std::string_view groupName, std::string_view configName, T val);

		// ����
		static void Save();

	private:
		ConfigSys() = default;
		static void Save(std::string_view name, ConfigGroup& group);

	private:

		sigslot::signal_st<> siganlOnSettingChange;
		std::map<std::string, ConfigGroup>  groups;	// map<group, json>
	};
	
}

// impl
namespace Mrk
{
	template<typename T>
	inline T ConfigSys::Get(std::string_view groupName, std::string_view configName, T defaultVal)
	{
		static_assert(std::is_arithmetic_v<T> || std::is_same_v<T, std::string>, "Config Type Not Support !");

		auto ret = Instance().groups.find(groupName.data());
		if (ret != Instance().groups.end())
		{
			if (ret->second.romote.IsObject() && ret->second.local.HasMember(configName.data()))
			{
				auto& config = ret->second.romote[configName.data()];
				return config.Get<T>();
			}
		}

		return defaultVal;
	}
	template<typename T>
	inline void ConfigSys::Set(std::string_view groupName, std::string_view configName, T val)
	{
		static_assert(std::is_arithmetic_v<T> || std::is_same_v<T, std::string>, "Config Type Not Support !");

		auto& group = Instance().groups.try_emplace(groupName.data(), Json::Document(Json::ObjectType)).first->second;
		group.local[configName.data()] = Json::Value(val);
		group.isDirty = true;
	}
	template<typename T>
	inline T ConfigSys::GetOrSet(std::string_view groupName, std::string_view configName, T defaultVal)
	{
		static_assert(std::is_arithmetic_v<T> || std::is_same_v<T, char*> || std::is_same_v<T, std::string>, "Config Type Not Support !");

		auto ret = Instance().groups.try_emplace(groupName.data(), ConfigGroup());
		auto& group = ret.first->second;

		if (ret.second)
		{
			std::filesystem::path path = std::filesystem::current_path() / Global::configDirPath.c_str() / std::format("{}{}", groupName, ".json");
			if (std::filesystem::exists(path))
			{
				auto ifstream = std::ifstream(path);
				assert(ifstream.is_open());

				std::string jstr = std::string(std::istreambuf_iterator<char>(ifstream), std::istreambuf_iterator<char>());

				if (group.romote.Parse(jstr.c_str()).HasParseError() || !group.romote.IsObject())
				{
					group = ConfigGroup();
				}
				else
				{
					group.local.CopyFrom(group.romote, group.local.GetAllocator());
				}
			}
		}

		if (group.romote.HasMember(configName.data()))
		{
			auto& config = group.romote[configName.data()];
			return config.Get<T>();
		}
		else
		{
			Json::Value newVal;
			if constexpr (std::is_same_v<T, std::string>)	// std::string�ػ�
			{
				newVal = Json::Value(defaultVal.c_str(), group.local.GetAllocator());
			}
			else
			{
				newVal = Json::Value(defaultVal, group.local.GetAllocator());
			}

			if (group.local.HasMember(configName.data()))
			{
				group.local[configName.data()] = newVal;
			}
			else
			{
				group.local.AddMember(Json::Value(configName.data(), group.local.GetAllocator()), newVal, group.local.GetAllocator());
			}

			Save(groupName, group);
			return defaultVal;
		}
	}
	template<typename T>
	inline T ConfigSys::GetAndSet(std::string_view groupName, std::string_view configName, T val)
	{
		static_assert(std::is_arithmetic_v<T> || std::is_same_v<T, std::string>, "Config Type Not Support !");

		auto& group = Instance().groups.try_emplace(groupName.data(), Json::Document(Json::ObjectType)).first->second;
		group[configName.data()] = Json::Value(val);

		return val;
	}
}