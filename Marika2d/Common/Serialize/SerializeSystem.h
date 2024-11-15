#pragma once

#include "Common/Singleton/Singleton.h"
#include "Common/Memory/MemCtrlSystem.h"

#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/stringbuffer.h"
#include "Third/rapidjson/document.h"
#include "Third/rapidjson/writer.h"

#include <unordered_map>
#include <type_traits>
#include <functional>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <list>
#include <map>

#ifndef MRK_JSONSTR_LENGTHMAX
#define MRK_JSONSTR_LENGTHMAX 99999	// to fix a bug that rapidjson returns a invalid string-jsonvalue but its string-flag is true
#endif // !MRK_JSONSTR_LENGTHMAX

#ifndef MRK_SERIALIZABLE
#define MRK_SERIALIZABLE(x) private: static inline bool _mrk_ ## x ## _serialize_register_  = []() { Mrk::SerializeSystem::RegisteClass<x>(#x); return true; }();
#endif // !MRK_SERIALIZABLE

#ifndef MRK_SERIALIZE_CLASS
#define MRK_SERIALIZE_CLASS(class) auto it = jobj.FindMember("_mrk_serialize_class_name_"); if (it != jobj.MemberEnd()) { it->name.SetString("_mrk_serialize_base_class_name_"); } jobj.AddMember("_mrk_serialize_class_name_", Json::Value(#class, jalloc), jalloc);
#endif // !MRK_SERIALIZE_CLASS

#ifndef MRK_SERIALIZE_BASED
#define MRK_SERIALIZE_BASED(base) base::Serialize(jobj, jalloc)
#endif // !MRK_SERIALIZE_BASED

#ifndef MRK_WRITE_FIELD
#define MRK_SERIALIZE_FIELD(field) jobj.AddMember(Json::Value(#field, jalloc), Mrk::JsonValueCreator::Create(field, jalloc), jalloc)
#endif // !MRK_SERIALIZE_FIELD

#ifndef MRK_DESERIALIZE_BASED
#define MRK_DESERIALIZE_BASED(base) base::DeSerialize(jobj);
#endif // !MRK_DESERIALIZE_BASED

#ifndef MRK_DESERIALIZE_FIELD
#define MRK_DESERIALIZE_FIELD(field, to) Mrk::JsonValueExtractor::Extract(jobj.FindMember(#field)->value, to);
#endif // !MRK_DESERIALIZE_FIELD

namespace Mrk
{
	using JsonAlloc = std::invoke_result_t<decltype(&Json::Document::GetAllocator), Json::Document*>;

	template <typename, typename = std::void_t<>> struct HasDeserialize : std::false_type {};
	template <typename T> struct HasDeserialize<T, std::void_t<decltype(&T::Func)>> : std::true_type {};

	template <typename, typename = std::void_t<>> struct HasSerialize : std::false_type {};
	template <typename T> struct HasSerialize<T, std::void_t<decltype(&T::Func)>> : std::true_type {};

	template <typename StrCaster, typename Str, typename = std::void_t<>> struct IsStrCaster : std::false_type {};
	template <typename StrCaster, typename Str> struct IsStrCaster<StrCaster, typename Str, std::void_t<decltype(std::declval<StrCaster&>().operator()(std::declval<const Str&>())), const char* >> : std::true_type {};

	template <typename Serializer, typename T, typename = std::void_t<>> struct IsSerializer : std::false_type {};
	template <typename Serializer, typename T> struct IsSerializer<Serializer, typename T, std::void_t<decltype(std::declval<Serializer&>().operator()(std::declval<const T&>(), std::declval<Json::Value&>(), std::declval<JsonAlloc&>())), Json::Value>> : std::true_type {};

	class ISerializable;

	class SerializeSystem : public Singleton<SerializeSystem>
	{
		MRK_SINGLETON(SerializeSystem)
	public:
		template<typename T>
		static void RegisteClass(std::string_view className);
		static ISerializable* Create(std::string_view className);
		
	private:
		SerializeSystem();
		
	private:
		std::map<std::string, std::function<ISerializable* ()>> creators;
	};

	class ISerializable
	{
	public:
		virtual inline void DeSerialize(const Json::Value& jobj) { }
		virtual inline void Serialize(Json::Value& jobj, JsonAlloc& jalloc) const {}
	};

	struct JsonValueCreator
	{
		// 处理 bool 类型
		template<typename T>
		std::enable_if_t<std::is_same_v<T, bool>, Json::Value>
			static Create(const T& _bool, JsonAlloc& jalloc)
		{
			return Json::Value(_bool);
		}

		// 处理 算数 类型
		template<typename T>
		std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, Json::Value>
			static Create(const T& num, JsonAlloc& jalloc)
		{
			return Json::Value(num);
		}

		// 处理 ISerializable 指针 
		template<typename T>
		std::enable_if_t<std::is_polymorphic_v<T>, Json::Value>
			static Create(const T* obj, JsonAlloc& jalloc)
		{
			Json::Value jobj(Json::Type::ObjectType);
			auto serializable = dynamic_cast<const ISerializable*>(obj);
			if (serializable)
			{
				serializable->Serialize(jobj, jalloc);
			}
			return jobj;
		}

		// 处理 ISerializable 引用 
		template<typename T>
		std::enable_if_t<std::is_polymorphic_v<T>, Json::Value>
			static Create(const T& obj, JsonAlloc& jalloc)
		{
			return Create(&obj, jalloc);
		}

		// 处理 ISerializable 智能指针 
		template<typename T>
		std::enable_if_t<std::is_polymorphic_v<T>, Json::Value>
			static Create(const std::unique_ptr<T> obj, JsonAlloc& jalloc)
		{
			return Create(obj.get(), jalloc);
		}

		// 处理 ISerializable 共享指针 
		template<typename T>
		std::enable_if_t<std::is_polymorphic_v<T>, Json::Value>
			static Create(const std::shared_ptr<T> obj, JsonAlloc& jalloc)
		{
			return Create(obj.get(), jalloc);
		}

		// 处理 ISerializable 弱指针 
		template<typename T>
		std::enable_if_t<std::is_polymorphic_v<T>, Json::Value>
			static Create(const std::weak_ptr<T> obj, JsonAlloc& jalloc)
		{
			if (!obj.expired())
			{
				return Create(obj.lock(), jalloc);
			}
			return Json::Value(Json::Type::ObjectType);
		}

		// 处理 带有序列化器 的 普通类型
		template<typename Serializer, typename T>
		std::enable_if_t<IsSerializer<Serializer, T>::value, Json::Value>
			static Create(const T& obj, JsonAlloc& jalloc)
		{
			Json::Value jobj(Json::Type::ObjectType);
			Serializer().operator()(obj, jobj, jalloc);
			return jobj;
		}

		// 处理 带有序列化器 的 普通类型 指针
		template<typename Serializer, typename T>
		std::enable_if_t<IsSerializer<Serializer, T>::value, Json::Value>
			static Create(const T* obj, JsonAlloc& jalloc)
		{
			if (obj)
			{
				return Create(*obj, jalloc);
			}
			return Json::Value(Json::Type::ObjectType);
		}

		// 处理 带有序列化器 的 普通类型 智能指针
		template<typename Serializer, typename T>
		std::enable_if_t<IsSerializer<Serializer, T>::value, Json::Value>
			static Create(const std::unique_ptr<T> obj, JsonAlloc& jalloc)
		{
			return Create(obj.get(), jalloc);
		}

		// 处理 带有序列化器 的 普通类型 共享指针
		template<typename Serializer, typename T>
		std::enable_if_t<IsSerializer<Serializer, T>::value, Json::Value>
			static Create(const std::shared_ptr<T> obj, JsonAlloc& jalloc)
		{
			return Create(obj.get(), jalloc);
		}

		// 处理 带有序列化器 的 普通类型 弱指针
		template<typename Serializer, typename T>
		std::enable_if_t<IsSerializer<Serializer, T>::value, Json::Value>
			static Create(const std::weak_ptr<T> obj, JsonAlloc& jalloc)
		{
			if (!obj.expired())
			{
				return Create(obj.lock(), jalloc);
			}
			return Json::Value(Json::Type::ObjectType);
		}

		// 处理 带有字符转换器 的 非stl字符串
		template<typename StrCaster, typename T>
		std::enable_if_t<std::is_constructible_v<T, const char*>&& IsStrCaster<StrCaster, T>::value, Json::Value>
			static Create(const T& str, JsonAlloc& jalloc)
		{
			return Json::Value(StrCaster().operator()(str), jalloc);
		}

		// 处理 stl str 或 const char*
		static Json::Value Create(std::string_view str, JsonAlloc& jalloc)
		{
			return Json::Value(str.data(), jalloc);
		}

		// 处理 vector
		template<typename T>
		static Json::Value Create(const std::vector<T>& elems, JsonAlloc& jalloc)
		{
			Json::Value jvec(Json::Type::ArrayType);
			for (auto& elem : elems)
			{
				auto jelem = Create(elem, jalloc);
				jvec.PushBack(jelem, jalloc);
			}
			return jvec;
		};

		// 处理 list
		template<typename T>
		static Json::Value Create(const std::list<T>& elems, JsonAlloc& jalloc)
		{
			Json::Value jlist(Json::Type::ArrayType);
			for (auto& elem : elems)
			{
				auto jelem = Create(elem, jalloc);
				jlist.PushBack(jelem, jalloc);
			}
			return jlist;
		};

		// 处理 map, 自定义类型 key 必须重载 >> 和 <<
		template<typename K, typename T>
		static Json::Value Create(const std::map<K, T>& elems, JsonAlloc& jalloc)
		{
			Json::Value jmap(Json::Type::ObjectType);
			for (auto& [key, value] : elems)
			{
				Json::Value jkey;
				if constexpr (std::is_arithmetic_v<K>)
				{
					jkey = Create(std::to_string(key).c_str(), jalloc);
				}
				else
				{
					std::stringstream ss; ss << key;
					jkey = Create(ss.str(), jalloc);
				}
				auto jval = Create(value, jalloc);
				jmap.AddMember(jkey, jval, jalloc);
			}
			return jmap;
		};

		// 处理 unordered_map, 自定义类型 key 必须重载 >> 和 <<
		template<typename K, typename T>
		static Json::Value Create(const std::unordered_map<K, T>& elems, JsonAlloc& jalloc)
		{
			Json::Value jumap(Json::Type::ObjectType);
			for (auto& [key, value] : elems)
			{
				Json::Value jkey;
				if constexpr (std::is_arithmetic_v<K>)
				{
					jkey = Create(std::to_string(key).c_str(), jalloc);
				}
				else
				{
					std::stringstream ss; ss << key;
					jkey = Create(ss.str(), jalloc);
				}
				auto jval = Create(value, jalloc);
				jumap.AddMember(jkey, jval, jalloc);
			}
			return jumap;
		};
	};

	struct JsonValueExtractor
	{
		//处理 bool 类型
		template<typename T>
		std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, void>
			static Extract(const Json::Value& jnum, T& num)
		{
			if (jnum.IsNumber())
			{
				if (jnum.IsDouble())
				{
					num = static_cast<T>(jnum.GetDouble());
				}
				else if (jnum.IsInt64())
				{
					num = static_cast<T>(jnum.GetInt64());
				}
				else if (jnum.IsUint64())
				{
					num = static_cast<T>(jnum.GetUint64());
				}
				return;
			}
			num = 0;
		}

		// 处理 算数 类型
		template<typename T>
		std::enable_if_t<std::is_same_v<T, bool>, void>
			static Extract(const Json::Value& jbool, T& _bool)
		{
			if (jbool.IsBool())
			{
				_bool = jbool.GetBool();
				return;
			}
			_bool = false;
		}

		// 处理 ISerializable 指针 
		template<typename T>
		std::enable_if_t<std::is_polymorphic_v<T>, void>
			static Extract(const Json::Value& jobj, T*& obj)
		{
			obj = nullptr;
			if (jobj.IsObject())
			{
				auto jret = jobj.FindMember("_mrk_serialize_class_name_");
				if (jret != jobj.MemberEnd() && jret->value.IsString())
				{
					auto serializable = SerializeSystem::Create(jret->value.GetString());
					if (serializable)
					{
						serializable->DeSerialize(jobj);
						obj = dynamic_cast<T*>(serializable);
					}
				}
			}
		}

		// 处理 ISerializable 引用 
		template<typename T>
		std::enable_if_t<std::is_polymorphic_v<T>, void>
			static Extract(const Json::Value& jobj, T& obj)
		{
			Extract(jobj, &obj);
		}

		// 处理 ISerializable 智能指针
		template<typename T>
		std::enable_if_t<std::is_polymorphic_v<T>, void>
			static Extract(const Json::Value& jobj, std::unique_ptr<T>& uobj)
		{
			uobj = std::make_unique<T>();
			Extract(jobj, uobj.get());
		}

		// 处理 ISerializable 共享指针 
		template<typename T>
		std::enable_if_t<std::is_polymorphic_v<T>, void>
			static Extract(const Json::Value& jobj, std::shared_ptr<T>& sobj)
		{
			//T* vobj = nullptr;
			//Extract(jobj, vobj);
			//sobj.reset(vobj);
			
			//修改，现在构造到对象池(可能)
			sobj = MemCtrlSystem::CreateNew<T>();
		}

		// 处理 stl str
		static void Extract(const Json::Value& jstr, std::string& str)
		{
			
			if (jstr.IsString() && jstr.GetStringLength() < MRK_JSONSTR_LENGTHMAX)
			{
				str = jstr.GetString();
				return;
			}
			str = "";
		}

		// 处理 vector
		template<typename T>
		static void Extract(const Json::Value& jvec, std::vector<T>& elems)
		{
			elems.clear();
			if (jvec.IsArray())
			{
				for (auto it = jvec.Begin(); it != jvec.End(); it++)
				{
					T elem;
					Extract(*it, elem);
					elems.push_back(elem);
				}	
			}
		};

		// 处理 list
		template<typename T>
		static void Extract(const Json::Value& jvec, std::list<T>& elems)
		{
			elems.clear();
			if (jvec.IsArray())
			{
				for (auto it = jvec.Begin(); it != jvec.End(); it++)
				{
					T elem;
					Extract(*it, elem);
					elems.push_back(elem);
				}
			}
		};

		// 处理 map, 自定义类型 key 必须重载 >> 和 <<
		template<typename K, typename T>
		static void Extract(const Json::Value& jmap, std::map<K, T>& elems)
		{
			elems.clear();
			if (jmap.IsObject())
			{
				for (auto it = jmap.MemberBegin(); it != jmap.MemberEnd(); it++)
				{
					K key;
					std::stringstream ss(it->name.GetString());
					ss >> key;
					T val;
					Extract(it->value, val);
					elems.try_emplace(key, val);
				}
			}
		};

		// 处理 unordered_map, 自定义类型 key 必须重载 >> 和 <<
		template<typename K, typename T>
		static void Extract(const Json::Value& jmap, std::unordered_map<K, T>& elems)
		{
			elems.clear();
			if (jmap.IsObject())
			{
				for (auto it = jmap.MemberBegin(); it != jmap.MemberEnd(); it++)
				{
					K key;
					std::stringstream ss(it->name.GetString());
					ss >> key;
					T val;
					Extract(it->value, val);
					elems.try_emplace(key, val);
				}
			}
		};
	};
}

namespace Mrk
{
	template<typename T>
	inline void SerializeSystem::RegisteClass(std::string_view className)
	{
		static_assert(std::is_base_of<ISerializable, T>::value, "T Is Not ISerializable !");
		auto ret = Instance().creators.try_emplace(className.data(),
			[]() -> ISerializable* {
				return new T();
			});
		if (!ret.second)
		{
			throw;
			//LogSystem::Log(LogLevel::Worning, __FILE__, __FUNCTION__, std::format("Class [{}] Registe Repeatly !", className));
		}
	}
}
