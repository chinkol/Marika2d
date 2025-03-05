#include "ConfigSys.h"

#include <filesystem>
#include <fstream>

rttr::variant Mrk::ConfigSys::GetConfigItemVariant(std::string_view group, std::string_view name)
{
	

	return rttr::variant();
}

namespace
{
	void ReadConfig(const std::filesystem::path& path, Mrk::ConfigGroup* group)
	{
		std::ifstream ifstream(path);

		std::string json((std::istreambuf_iterator<char>(ifstream)), std::istreambuf_iterator<char>());
		ifstream.close();

		Json::Document jdoc;
		jdoc.Parse(json);

		group->FromJson(jdoc);
	}

	void WriteConfig(const std::filesystem::path& path, Mrk::ConfigGroup* group)
	{
		std::ofstream oftream(path);

		Json::Document jdoc;
		Json::StringBuffer buf;
		Json::PrettyWriter writer(buf);

		auto json = group->ToJson(jdoc.GetAllocator());
		json.Accept(writer);

		oftream << buf.GetString();
		oftream.close();
	}
}

void Mrk::ConfigSys::Init()
{
	for (auto& ctor : Instance().ctors)
	{
		Instance().groups.try_emplace(ctor.first, [ctor](){
			auto group = ctor.second();

			std::filesystem::path dir = MRK_CONFIG_PATH;
			std::filesystem::path path = dir / ctor.first;
			path.replace_extension(MRK_CONFIG_EXTENSION);
			
			if (!std::filesystem::exists(dir))
			{
				std::filesystem::create_directories(dir);
			}

			if (std::filesystem::exists(path))
			{
				ReadConfig(path, group);
				WriteConfig(path, group);	// 先读入再写入，确保新增成员的默认值可以保存, 以及被错误保存或读取的成员变量值可以恢复
			}
			else
			{
				WriteConfig(path, group);
			}
			
			return group;
			}());

		Instance().defaults.try_emplace(ctor.first, [ctor]() {
			return ctor.second();
			}());
	}
}
