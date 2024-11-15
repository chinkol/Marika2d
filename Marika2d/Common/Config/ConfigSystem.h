#pragma once

#include <string>

namespace Mrk
{
	class ConfigSystem
	{
	public:
		static void Init();

	private:
		static ConfigSystem* instance;

	public:
		~ConfigSystem() = default;

	private:
		ConfigSystem() = default;

	private:
		std::string logDir;
	};
}


