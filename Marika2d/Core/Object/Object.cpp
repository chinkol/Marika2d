#include "Object.h"

Json::Value Mrk::Object::ToJson(Mrk::JsonAllocator& alloctor)
{
    return Json::Value(Json::ObjectType);
}

void Mrk::Object::FromJson(const Json::Value& json)
{
}