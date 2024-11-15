#include "ConfigSystem.h"

#include "../Log/LogSystem.h"

Mrk::ConfigSystem* Mrk::ConfigSystem::instance = nullptr;

void Mrk::ConfigSystem::Init()
{
	if (!instance)
	{
		instance = new ConfigSystem();
	}
	else
	{
		MRK_LOG_WORNING("ConfigSystem::Init() Be Called Many Times ?");
	}
}
