//#include "SerializeSystem.h"
//
//#include "../Log/LogSystem.h"
//
//#include <assert.h>
//
//Mrk::SerializeManager& Mrk::SerializeManager::GetInstance()
//{
//	static SerializeManager instance;
//	return instance;
//}
//
//Mrk::ISerializable* Mrk::SerializeManager::Create(std::string_view className)
//{
//	auto ret = creaters.find(className.data());
//	if (ret != creaters.end())
//	{
//		return ret->second();
//	}
//	else
//	{
//		MRK_LOG_ERROR(std::format("Class [{}] Not Be Registed !", className));
//		throw std::runtime_error(std::format("Class [{}] Not Be Registed !", className));
//	}
//}
//
//Mrk::SerializeStream::~SerializeStream()
//{
//	for (auto& [_, field] : fields)
//	{
//		delete field;
//		field = nullptr;
//	}
//}
//
//
//Mrk::OutSerializeStream::OutSerializeStream(SerializeStream* stream) : 
//	stream(stream)
//{
//	assert(stream);
//}
//
//void Mrk::OutSerializeStream::WriteField(std::string_view fieldName, const ISerializable* value)
//{
//	assert(value);
//
//	SerializeStream* s = new SerializeStream();
//	OutSerializeStream os(s);
//	value->Serialize(&os);
//
//	auto ret = stream->subStreams.try_emplace(fieldName.data(), s);
//	if (!ret.second)
//	{
//		MRK_LOG_ERROR(std::format("FieldName [{}] Repeat !", fieldName));
//		throw std::runtime_error(std::format("FieldName [{}] Repeat !", fieldName));
//	}
//}
//
//Mrk::InSerializeStream::InSerializeStream(SerializeStream* stream) : 
//	stream(stream)
//{
//	assert(stream);
//}
//
//void Mrk::JsonSerializer::ToFile(ISerializable* serializable)
//{
//	SerializeStream s;
//
//	OutSerializeStream os(&s);
//	serializable->Serialize(&os);
//
//	for (auto& [name, field] : s.fields)
//	{
//		if (field->IsBaseType())
//		{
//			int t = 0;
//		}
//	}
//
//	//configor::json::
//}
//
//Mrk::ISerializable* Mrk::JsonSerializer::FromFile(std::string_view filename)
//{
//	return nullptr;
//}
//
//const type_info& Mrk::SerializeFieldBase::GetType()
//{
//	return value.type();
//}
//
//const std::any& Mrk::SerializeFieldBase::GetValue()
//{
//	return value;
//}
//
//void Mrk::SerializeFieldBase::SetValue(const std::any& value)
//{
//	if (this->value.type() == value.type())
//	{
//		this->value = value;
//	}
//	else
//	{
//		MRK_LOG_ERROR(std::format("Type [{}] Cast Failed !", value.type().raw_name()));
//		throw std::runtime_error("Type Cast Failed !");
//	}
//}
//
//const type_info& Mrk::SerializeField_::GetType()
//{
//	return value.type();
//}
//
//const std::any& Mrk::SerializeField_::GetValue()
//{
//	return value;
//}
//
//void Mrk::SerializeField_::SetValue(const std::any& value)
//{
//	this->value = value;
//}
