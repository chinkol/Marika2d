#include "AssetSystem.h"

const std::vector<std::string>& Mrk::AssetSystem::GetImportSupports()
{
	static auto supportsView = Instance().creators | std::ranges::views::transform([](const auto& pair) { return pair.first; });
	static auto supports = std::vector<std::string>(supportsView.begin(), supportsView.end());

	return supports;
}

void Mrk::AssetSystem::Import(const std::filesystem::path& from, const std::filesystem::path& to)
{
	auto creator = Instance().creators.find(from.extension().string());
	if (creator != Instance().creators.end())
	{
		auto importer = creator->second();
		importer->Import(from, to);
		delete importer;
	}
	else
	{
		throw; //unsupport file type
	}
}
