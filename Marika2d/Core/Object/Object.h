#pragma once

#include <string>
#include <typeinfo>

#include "Core/Reflect/Reflect.h"

#ifndef MRK_OBJECT
#define MRK_OBJECT(x)														\
public:																		\
virtual inline std::string_view GetClassName() { return #x; }				\
virtual inline const type_info& GetClassType() { return typeid(x); }		\
static inline std::string_view GetStaticClassName() { return #x; }			\
static inline const type_info& GetStaticClassType() { return typeid(x); }	
#endif // !MRK_OBJECT

namespace Mrk
{
	class Object
	{
		MRK_OBJECT(Object)
	public:
		virtual ~Object() = default;
		virtual Json::Value ToJson(Mrk::JsonAllocator& alloctor);
		virtual void FromJson(const Json::Value& json);

		RTTR_ENABLE()
	};
}