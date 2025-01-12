#include "ConfigSys.h"

inline Mrk::ConfigSys::ConfigGroup::ConfigGroup() :
	romote(Json::ObjectType),
	local(Json::ObjectType),
	isDirty(false)
{

}

const std::map<std::string, Mrk::ConfigSys::ConfigGroup>& Mrk::ConfigSys::GetConfigGroups()
{
	return Instance().groups;
}

inline void Mrk::ConfigSys::Save()
{
	for (auto& [name, group] : Instance().groups)
	{
		if (group.isDirty)
		{
			Save(name, group);
			group.isDirty = false;
		}
	}
}

inline void Mrk::ConfigSys::Save(std::string_view name, ConfigGroup& group)
{
	auto path = std::filesystem::current_path() / Mrk::Global::configDirPath / std::format("{}{}", name, ".json");

	if (!std::filesystem::exists(path.parent_path()))
	{
		std::filesystem::create_directories(path.parent_path());
	}

	auto ofstream = std::ofstream(path);
	assert(ofstream.is_open());

	group.romote.CopyFrom(group.local, group.romote.GetAllocator());

	Json::StringBuffer buffer;
	Json::PrettyWriter<Json::StringBuffer> writer(buffer);
	group.romote.Accept(writer);

	ofstream << buffer.GetString();
}
