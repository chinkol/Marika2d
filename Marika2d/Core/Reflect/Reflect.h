#pragma once

#include "Common/Singleton/Singleton.h"

#include "Third/rapidjson/document.h"
#include "Third/rapidjson/prettywriter.h"
#include "Third/rttr/registration.h"

#include <string>
#include <memory>
#include <vector>

#ifndef MRK_REFLECT_CLASS_JSON_PROP_NAME
#define MRK_REFLECT_CLASS_JSON_PROP_NAME "_mrk_reflect_class_name_"
#endif // !MRK_REFLECT_CLASS_JSON_PROP_NAME

#ifndef _MRK_RTTR_TYPE_CONTENT
#define _MRK_RTTR_TYPE_CONTENT(x) { static inline bool _mrk_macro_type_reflect_rigester_ = [](){ x return true; }(); }
#endif // !_MRK_RTTR_TYPE_CONTENT

#ifndef MRK_RTTR_TYPE
#define MRK_RTTR_TYPE(x) namespace _mrk_rttr_type_##x##_reflect_register_namespace_ _MRK_RTTR_TYPE_CONTENT
#endif // !_MRK_RTTR_TYPE

#ifndef MRK_RTTR_BASIC_TYPE
#define MRK_RTTR_BASIC_TYPE(x) rttr::registration::class_<x>(#x).constructor<>()(rttr::policy::ctor::as_object);
#endif // !MRK_RTTR_BASIC_ONLY_CTOR

namespace Mrk
{
	using JsonAllocator = Json::MemoryPoolAllocator<Json::CrtAllocator>;
}

namespace Mrk
{
	class ReflectSys : public Mrk::Singleton<ReflectSys>
	{
		MRK_SINGLETON(ReflectSys)
	private:
		ReflectSys() = default;
	public:
		static Json::Value ToJson(rttr::instance obj, Mrk::JsonAllocator& alloctor);
		static void FromJson(rttr::instance obj, const Json::Value& jobj);
	};
}