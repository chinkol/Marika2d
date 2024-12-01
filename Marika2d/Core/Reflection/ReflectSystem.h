#pragma once

#include "Common/Singleton/Singleton.h"

#include <vector>
#include <string>
#include <typeindex>
#include <functional>
#include <map>

#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/stringbuffer.h"
#include "Third/rapidjson/document.h"
#include "Third/rapidjson/writer.h"

namespace MrkNew
{
	using JsonAlloc = std::invoke_result_t<decltype(&Json::Document::GetAllocator), Json::Document*>;
	using Serializer = std::function<void(Json::Value& jval, JsonAlloc& jalloc)>;
	using DeSerializer = std::function<void(const Json::Value& jval)>;

	class SerializeSystem : public Mrk::Singleton<SerializeSystem>
	{
		MRK_SINGLETON(SerializeSystem)
	public:
		
	};

	class FieldInfoBase;

	class ClassInfo
	{
	public:
		template<typename F> void RegisterField(std::string_view fieldname, Serializer serializer, DeSerializer deserializer)
		{

		}
	protected:
		std::map<std::string, FieldInfoBase*> fields;
	};

	class FieldInfoBase
	{

	};

	template<typename T, typename F>
	class FieldInfo : public FieldInfoBase
	{
	public:
		Serializer serializer;
		DeSerializer deserializer;
	};
}

namespace MrkTest
{
	class ReflectTest
	{
		friend class ReflectTestReflect;
	public:
		int field1 = 123;
		double field2 = 1.23;
		std::vector<std::string> field3 = { "Hello", "World" };
	private:
		float filed4 = 123.4f;
	};

	class ReflectTestReflect
	{
	public:
		void operator() (MrkNew::ClassInfo& classInfo)
		{
			MrkNew::Serializer serializer = [](Json::Value& jval, MrkNew::JsonAlloc& jalloc) {
				
				};

			MrkNew::DeSerializer deserializer = [](const Json::Value& jval) {

				};

			classInfo.RegisterField<ReflectTest>("field1", serializer, deserializer);
		}

		void operator() (const Json::Value& jval)
		{

		}
	};
}


