#pragma once

#include"../Def/Def.h"

#include <map>
#include <assert.h>
#include <string>
#include <optional>
#include <typeindex>
#include <type_traits>
#include <functional>

#ifndef MRK_WRITE_FIELD
#define MRK_WRITE_FIELD(x)	stream->WriteField(#x, x);
#endif // !MRK_WRITE_FIELD

#ifndef MRK_READ_FIELD
#define MRK_READ_FIELD(x, y) stream->ReadField(#x, y);
#endif // !MRK_READ_FIELD

#ifndef MRK_SERIALIZABLE
#define MRK_SERIALIZABLE(x) private: static inline bool _ ## x ## _serialize_register_ = []() { Mrk::SerializeManager::GetInstance().RegisteClass<x>(#x); return true; }();
#endif // !MRK_SERIALIZABLE

namespace Mrk
{
	class ISerializable;

	class SerializeManager
	{
	public:
		static SerializeManager& GetInstance();
		template<typename T> void RegisteClass(std::string_view className);
		ISerializable* Create(std::string_view className);
	private:
		SerializeManager() = default;

	private:
		std::map<std::string, std::function<ISerializable* ()>> creaters;
	};

	class SerializeFieldBase
	{
		friend class SerializeStream;
		friend class OutSerializeStream;
		friend class InSerializeStream;
	public:
		virtual ~SerializeFieldBase() = default;
		virtual void AssignmentCopyTo(void* target) = 0;
	protected:
		std::type_index type = typeid(nullptr);
	};

	template<typename T>
	class SerializeField : public SerializeFieldBase
	{
		friend class SerializeStream;
		friend class OutSerializeStream;
		friend class InSerializeStream;
	public:
		SerializeField(const T& value);
		virtual void AssignmentCopyTo(void* target) override;
	private:
		T value;
	};

	class SerializeStream
	{
		friend class OutSerializeStream;
		friend class InSerializeStream;
	public:
		~SerializeStream();
		std::map<std::string, SerializeFieldBase*>& GetFields();
	private:
		std::map<std::string, SerializeFieldBase*> fields;
	};

	class ISerializable;

	class OutSerializeStream
	{
	public:
		OutSerializeStream(SerializeStream* stream);
		template<typename T> void WriteField(std::string_view fieldName, const T& value);
	private:
		SerializeStream* stream;
	};

	class InSerializeStream
	{
	public:
		InSerializeStream(SerializeStream* stream);
		template<typename T> void ReadField(std::string_view fieldName, T& value);
	private:
		SerializeStream* stream;
	};

	class ISerializable
	{
		virtual inline void Serialize(OutSerializeStream* stream) {};
		virtual inline void DeSerialize(InSerializeStream* stream) {};
	};
}

#include <format>

#include "../Log/LogSystem.h"

namespace Mrk
{
	template<typename T>
	inline SerializeField<T>::SerializeField(const T& value) : 
		value(value)
	{
	}

	template<typename T>
	inline void SerializeField<T>::AssignmentCopyTo(void* target)
	{
		*(T*)target = value;
	}

	template<typename T>
	inline void SerializeManager::RegisteClass(std::string_view className)
	{
		static_assert(std::is_base_of<ISerializable, T>::value, "T Is Not ISerializable !");
		auto ret = creaters.try_emplace(className.data(), 
			[]() -> ISerializable* { 
				return new T(); 
			});
		if (!ret.second)
		{
			LogSystem::Log(LogLevel::Worning, __FILE__, __FUNCTION__, std::format("Class [{}] Registe Repeatly !", className));
		}
	}

	template<typename T>
	inline void Mrk::OutSerializeStream::WriteField(std::string_view fieldName, const T& value)
	{
		static_assert(std::is_copy_assignable_v<T>, "Copy Assignment Func Has Been Deleted !");
		static_assert(std::is_copy_constructible_v<T>, "Copy Construct Func Has Been Deleted !");

		auto field = new SerializeField<T>(value);
		field->type = typeid(T);

		auto ret = stream->fields.try_emplace(fieldName.data(), field);
		if (!ret.second)
		{
			MRK_LOG_ERROR("FieldName Repeat");
			throw std::runtime_error("FieldName Repeat");
		}
	}

	template<typename T>
	inline void InSerializeStream::ReadField(std::string_view fieldName, T& value)
	{
		auto ret = stream->fields.find(fieldName.data());
		if (ret != stream->fields.end())
		{
			if (ret->second->type == typeid(T))
			{
				ret->second->AssignmentCopyTo(&value);
			}
			else
			{
				MRK_LOG_ERROR("Field Type Convert Failed !");
				throw std::runtime_error("Field Type Convert Failed !");
			}
		}
		else
		{
			LogSystem::Log(Mrk::LogLevel::Error, __FILE__, __FUNCTION__, std::format("Field [{}] Not Exist !", fieldName));
			throw std::runtime_error(std::format("Field [{}] Not Exist !", fieldName));
		}
	}
}


