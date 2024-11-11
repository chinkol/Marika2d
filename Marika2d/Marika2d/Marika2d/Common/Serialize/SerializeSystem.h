//#pragma once
//
//#include <map>
//#include <any>
//#include <assert.h>
//#include <string>
//#include <optional>
//#include <typeindex>
//#include <type_traits>
//#include <functional>
//
//#ifndef MRK_WRITE_FIELD
//#define MRK_WRITE_FIELD(x)	stream->WriteField(#x, x);
//#endif // !MRK_WRITE_FIELD
//
//#ifndef MRK_READ_FIELD
//#define MRK_READ_FIELD(x, y) stream->ReadField(#x, y);
//#endif // !MRK_READ_FIELD
//
//#ifndef MRK_SERIALIZABLE
//#define MRK_SERIALIZABLE(x) private: static inline bool _ ## x ## _Serialize_Register_Dont_Use_Me_ = []() { Mrk::SerializeManager::GetInstance().RegisteClass<x>(#x); return true; }();
//#endif // !MRK_SERIALIZABLE
//
//namespace Mrk
//{
//	class ISerializable;
//
//	class SerializeManager
//	{
//	public:
//		static SerializeManager& GetInstance();
//		template<typename T> void RegisteClass(std::string_view className);
//		ISerializable* Create(std::string_view className);
//		//void ToFile(ISerializable* serializable);
//	private:
//		SerializeManager() = default;
//
//	private:
//		std::map<std::string, std::function<ISerializable* ()>> creaters;
//	};
//
//	class SerializeFieldBase
//	{
//		friend class SerializeStream;
//		friend class OutSerializeStream;
//		friend class InSerializeStream;
//	public:
//		const type_info& GetType();
//		const std::any& GetValue();
//		void SetValue(const std::any& value);
//
//		virtual ~SerializeFieldBase() = default;
//		virtual bool IsArray() = 0;
//		virtual bool IsBaseType() = 0;
//		virtual bool IsDictionary() = 0;
//		virtual bool IsSerializable() = 0;
//		virtual void AssignmentCopyTo(void* target) = 0;
//	protected:
//		std::any value;
//	};
//
//	template<typename T>
//	class SerializeField final : public SerializeFieldBase
//	{
//		friend class SerializeStream;
//		friend class OutSerializeStream;
//		friend class InSerializeStream;
//	public:
//		SerializeField(const T& value);
//		virtual bool IsArray() override;
//		virtual bool IsBaseType() override;
//		virtual bool IsDictionary() override;
//		virtual bool IsSerializable() override;
//		virtual void AssignmentCopyTo(void* target) override;
//	};
//
//	class SerializeField_
//	{
//	public:
//		const type_info& GetType();
//		const std::any& GetValue();
//		void SetValue(const std::any& value);
//	protected:
//		std::any value;
//	};
//
//	template<typename T> 
//	class SerializeObject : public SerializeField_
//	{
//
//	};
//
//	struct SerializeStream
//	{
//		~SerializeStream();
//		std::map<std::string, SerializeFieldBase*> fields;
//		std::map<std::string, SerializeStream*> subStreams;
//	};
//
//	class ISerializable;
//
//	class OutSerializeStream
//	{
//	public:
//		OutSerializeStream(SerializeStream* stream);
//		void WriteField(std::string_view fieldName, const ISerializable* value);
//		template<typename T> void WriteField(std::string_view fieldName, const T& value);
//	private:
//		SerializeStream* stream;
//	};
//
//	class InSerializeStream
//	{
//	public:
//		InSerializeStream(SerializeStream* stream);
//		template<typename T> void ReadField(std::string_view fieldName, T& value);
//	private:
//		SerializeStream* stream;
//	};
//
//	class ISerializable
//	{
//	public:
//		virtual inline void Serialize(OutSerializeStream* stream) const {};
//		virtual inline void DeSerialize(InSerializeStream* stream) {};
//	};
//
//	class ISerializer
//	{
//	public:
//		virtual void ToFile(ISerializable* serializable) = 0;
//		virtual ISerializable* FromFile(std::string_view filename) = 0;
//	};
//
//	class JsonSerializer final : public ISerializer
//	{
//	public:
//		virtual void ToFile(ISerializable* serializable) override;
//		virtual ISerializable* FromFile(std::string_view filename) override;
//	};
//}
//
//#include <format>
//
//#include "../Log/LogSystem.h"
//
//namespace Mrk
//{
//	template<typename T>
//	inline SerializeField<T>::SerializeField(const T& value) 
//	{
//		this->value = value;
//	}
//
//	template<typename T>
//	inline bool SerializeField<T>::IsArray()
//	{
//		return false;
//	}
//
//	template<typename T>
//	inline bool SerializeField<T>::IsBaseType()
//	{
//		return std::is_same_v<T, bool>
//			|| std::is_same_v<T, int32_t>
//			|| std::is_same_v<T, int64_t>
//			|| std::is_same_v<T, float_t>
//			|| std::is_same_v<T, double_t>
//			|| std::is_same_v<T, uint16_t>
//			|| std::is_same_v<T, uint32_t>
//			|| std::is_same_v<T, uint64_t>
//			|| std::is_same_v<T, std::string>;
//	}
//
//	template<typename T>
//	inline bool SerializeField<T>::IsDictionary()
//	{
//		return false;
//	}
//
//	template<typename T>
//	inline bool SerializeField<T>::IsSerializable()
//	{
//		return std::is_base_of_v<ISerializable, T>;
//	}
//
//	template<typename T>
//	inline void SerializeField<T>::AssignmentCopyTo(void* target)
//	{
//		*(T*)target = std::any_cast<T>(value);
//	}
//
//	template<typename T>
//	inline void SerializeManager::RegisteClass(std::string_view className)
//	{
//		static_assert(std::is_base_of<ISerializable, T>::value, "T Is Not ISerializable !");
//		auto ret = creaters.try_emplace(className.data(), 
//			[]() -> ISerializable* { 
//				return new T(); 
//			});
//		if (!ret.second)
//		{
//			LogSystem::Log(LogLevel::Worning, __FILE__, __FUNCTION__, std::format("Class [{}] Registe Repeatly !", className));
//		}
//	}
//
//	template<typename T>
//	inline void Mrk::OutSerializeStream::WriteField(std::string_view fieldName, const T& value)
//	{
//		static_assert(!std::is_pointer_v<T>, "T Cant Be A Pointer !");
//		static_assert(std::is_copy_assignable_v<T>, "Copy Assignment Func Has Been Deleted !");
//		static_assert(std::is_copy_constructible_v<T>, "Copy Construct Func Has Been Deleted !");
//
//		auto ret = stream->fields.try_emplace(fieldName.data(), new SerializeField<T>(value));
//		if (!ret.second)
//		{
//			MRK_LOG_ERROR(std::format("FieldName [{}] Repeat !", fieldName));
//			throw std::runtime_error(std::format("FieldName [{}] Repeat !", fieldName));
//		}
//	}
//
//	template<typename T>
//	inline void InSerializeStream::ReadField(std::string_view fieldName, T& value)
//	{
//		auto ret = stream->fields.find(fieldName.data());
//		if (ret != stream->fields.end())
//		{
//			if (ret->second->GetType() == typeid(T))
//			{
//				ret->second->AssignmentCopyTo(&value);
//			}
//			else
//			{
//				MRK_LOG_ERROR("Field Type Convert Failed !");
//				throw std::runtime_error(std::format("Field Type [{}] Convert Failed !", typeid(T).raw_name()));
//			}
//		}
//		else
//		{
//			MRK_LOG_ERROR(std::format("Field [{}] Not Exist !", fieldName));
//			throw std::runtime_error(std::format("Field [{}] Not Exist !", fieldName));
//		}
//	}
//}
//
//
