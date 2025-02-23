#include "Object.h"

Json::Value Mrk::Object::ToJson(Mrk::JsonAllocator& alloctor)
{
    return Json::Value(Json::ObjectType);
}

void Mrk::Object::FromJson(const Json::Value& json)
{
}

std::string_view Mrk::Object::GetClassTypeName() 
{
    return "Object";
}

const type_info& Mrk::Object::GetClassType() 
{ 
    return typeid(Object); 
}