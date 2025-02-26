#pragma once

#include <string>
#include <typeinfo>

#include "Core/Reflect/Reflect.h"

#ifndef MRK_OBJECT
#define MRK_OBJECT(x)															\
public:																			\
virtual inline std::string_view GetClassTypeName() override { return #x; }		\
virtual inline const type_info& GetClassType() override { return typeid(x); }	\
static inline std::string_view StaticGetClassTypeName() { return #x; }			
#endif // !MRK_OBJECT

#ifndef MRK_OBJECT_SERIALIZE_SELF
#define MRK_OBJECT_SERIALIZE_SELF(x) public:																			\
virtual Json::Value ToJson(Mrk::JsonAllocator& alloctor) override { return Mrk::ReflectSys::ToJson(*this, alloctor); }	\
virtual void FromJson(const Json::Value& json) override { Mrk::ReflectSys::FromJson(*this, json); }
#endif // MRK_OBJECT_SERIALIZE_SELF

namespace Mrk
{
	class Object
	{
		RTTR_ENABLE()
	public:
		virtual ~Object() = default;
		virtual Json::Value ToJson(Mrk::JsonAllocator& alloctor);
		virtual void FromJson(const Json::Value& json);
		virtual std::string_view GetClassTypeName();
		virtual const type_info& GetClassType();
	};
}