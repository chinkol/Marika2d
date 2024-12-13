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
#define MRK_REFL_TYPE(x) static inline bool _mrk_macro_type_reflect_rigester_ = [](){ rttr::registration::class_<x>(#x)
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
		static std::string ToJson(rttr::instance obj);
	private:
	};
}


