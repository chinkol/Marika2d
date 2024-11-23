#include "AssetSystem.h"

const std::vector<std::string>& Mrk::AssetSystem::GetImportSupports()
{
	static auto supportsView = Instance().creators | std::ranges::views::transform([](const auto& pair) { return "." + pair.first; });
	static auto supports = std::vector<std::string>(supportsView.begin(), supportsView.end());

	return supports;
}
