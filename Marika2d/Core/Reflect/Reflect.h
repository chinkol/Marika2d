#pragma once

#include "Third/rttr/registration.h"
#include "Third/rapidjson/rapidjson.h"
#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/document.h"

#include "Common/Singleton/Singleton.h"

#ifndef MRK_REFL_BASE
#define MRK_REFL_BASE(x) RTTR_ENABLE(x)
#endif // !MRK_REFL_BASE

#ifndef MRK_REFL_TYPE
#define MRK_REFL_TYPE(x) bool _mrk_macro_type_reflect_rigester_ = [](){ rttr::registration::class_<shape>(#x)
#endif // !MRK_REFL_TYPE

#ifndef MRK_REFL_CONS
#define MRK_REFL_CONS .constructor()
#endif // !MRK_REFL_CONS

#ifndef MRK_REFL_PROP
#define MRK_REFL_PROP .property
#endif // !MRK_REFL_PROP

#ifndef MRK_REFL_READ
#define MRK_REFL_READ .property_readonly
#endif // !MRK_REFL_READ

#ifndef MRK_REFL_FUNC
#define MRK_REFL_FUNC .method
#endif // !MRK_REFL_FUNC

#ifndef MRK_REFL_META
#define MRK_REFL_META(x, y) ( rttr::metadata(x, y) )
#endif // !MRK_REFL_META

#ifndef MRK_REFL_REND
#define MRK_REFL_REND ; return true; }();
#endif // !MRK_REFL_REND

#include <iostream>

namespace MrkNew
{
	class ReflectSystem : public Mrk::Singleton< ReflectSystem>
	{
		MRK_SINGLETON(ReflectSystem)
	public:
		static inline std::string ToJson(rttr::instance obj)
		{
			if (!obj.is_valid())
				return std::string();

			Json::StringBuffer buffer;
			Json::PrettyWriter<Json::StringBuffer> writer(buffer);

			writer.StartObject();

			rttr::instance ins = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
			auto props = ins.get_derived_type().get_properties();
			for (auto prop : props)
			{
				rttr::variant value = prop.get_value(ins);
				if (value)
				{
					auto name = prop.get_name();
					writer.String(name.data(), static_cast<Json::SizeType>(name.length()), false);
				}
			}

			writer.EndObject();

			std::cout << buffer.GetString() << std::endl;
		}
	private:
	};
}


