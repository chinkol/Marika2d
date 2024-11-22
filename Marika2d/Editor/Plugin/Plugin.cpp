#include "Plugin.h"

void Mrk::PluginSystem::Init()
{
	Instance().plugins.clear();
	for (auto& creator : Instance().creators)
	{
		Instance().plugins.push_back(creator());
	}
}

void Mrk::PluginSystem::Update()
{
	for (auto& plugin : Instance().plugins)
	{
		plugin->Update();
	}
}

void Mrk::PluginSystem::Draw()
{
	for (auto& plugin : Instance().plugins)
	{
		plugin->Draw();
	}
}
