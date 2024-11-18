#include "Component.h"

std::shared_ptr<Mrk::Component> Mrk::ComponentFactory::CreateNew(std::string_view classname)
{
	auto ret = Instance().creators.find(classname.data());
	assert(ret != Instance().creators.end());
	return ret->second();
}

const std::map<std::string, std::function<std::shared_ptr<Mrk::Component>()>>& Mrk::ComponentFactory::GetCreators()
{
	return Instance().creators;
}

std::weak_ptr<Mrk::GameObject> Mrk::Component::GetHolder()
{
	return holder;
}

void Mrk::ComponentHouse::Invoke(std::string_view loopState)
{
	auto state = Instance().loopStates.find(loopState.data());
	if (state != Instance().loopStates.end())
	{
		for (auto& [_, callbacks] : state->second)
		{
			for (auto& callback : *callbacks)
			{
				if (!callback.Expired())
				{
					callback.Invoke();
				}
			}
		}
	}
}

Mrk::ComponentCallBack::ComponentCallBack(const std::shared_ptr<Component>& component, CallBack callback) :
	component(component),
	callback(callback)
{

}

void Mrk::ComponentCallBack::Invoke()
{
	auto scom = component.lock();
	//if (scom->Enable())
	{
		(scom.get()->*callback)();
	}
}

bool Mrk::ComponentCallBack::Expired()
{
	return component.expired();
}
