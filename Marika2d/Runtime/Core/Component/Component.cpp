#include "Component.h"

std::shared_ptr<Mrk::Component> Mrk::ComponentFactory::CreateNew(std::string_view classname)
{
	auto ret = Instance().creators.find(classname.data());
	assert(ret != Instance().creators.end());
	return ret->second();
}

std::weak_ptr<Mrk::GameObject> Mrk::Component::GetHolder()
{
	return holder;
}
