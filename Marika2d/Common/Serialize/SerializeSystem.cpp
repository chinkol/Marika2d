#include "SerializeSystem.h"

Mrk::ISerializable* Mrk::SerializeSystem::Create(std::string_view className)
{
    auto& creators = Instance().creators;
    auto ret = creators.find(className.data());
    if (ret != creators.end())
    {
        return ret->second();
    }
    //TODO : LOG : creator not found
    return nullptr;
}

Mrk::SerializeSystem::SerializeSystem()
{
}