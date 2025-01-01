#pragma once

#include "Third/rttr/registration.h"
#include "Third/rapidjson/rapidjson.h"
#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/document.h"

#include "Common/Singleton/Singleton.h"

#ifndef MRK_RTTR_BASE
#define MRK_RTTR_BASE(x) RTTR_ENABLE(x)
#endif // !MRK_RTTR_BASE

#ifndef _MRK_RTTR_TYPE_CONTENT
#define _MRK_RTTR_TYPE_CONTENT(x) { static inline bool _mrk_macro_type_reflect_rigester_ = [](){ x return true; }(); }
#endif // !_MRK_RTTR_TYPE_CONTENT

#ifndef MRK_RTTR_TYPE
#define MRK_RTTR_TYPE(x) namespace _mrk_macro_rttr_type_##x##_reflect_register_namespace_ _MRK_RTTR_TYPE_CONTENT
#endif // !_MRK_RTTR_TYPE

#include <iostream>

namespace MrkNew
{
	class ReflectSystem : public Mrk::Singleton<ReflectSystem>
	{
		MRK_SINGLETON(ReflectSystem)
	public:
		static std::string ToJson(rttr::instance obj);
		static rttr::variant FromJson(const std::string& json);
	private:
	};
}


