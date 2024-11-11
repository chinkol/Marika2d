#include "Serializer.h"

#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/stringbuffer.h"

#include <filesystem>
#include <iostream>

SerializeValue::~SerializeValue()
{
}

SerializeType SerializeValue::GetType()
{
	return this->type;
}

SerializeObject::SerializeObject()
{
	type = SerializeType::Object;
}

SerializeObject::SerializeObject(const ISerializable* obj) : 
	SerializeObject()
{
	WriteObj(obj);
}

void SerializeObject::WriteObj(const ISerializable* obj)
{
	members.clear();
	obj->Serialize(this);
}

void SerializeObject::ReadObj(ISerializable* obj)
{
	obj->DeSerialize(this);
}

void SerializeObject::AddMember(std::string_view memberName, SerializeValue* member)
{
	if (!members.try_emplace(memberName.data(), member).second)
	{
		throw;
	}
}

SerializeValue* SerializeObject::GetMember(std::string_view memberName)
{
	auto ret = members.find(memberName.data());

	if (ret == members.end())
	{
		throw;
	}

	return ret->second;
}

SerializeNumber::SerializeNumber() : 
	SerializeNumber(0)
{
}

SerializeNumber::~SerializeNumber()
{
	free(num);
}

std::type_index SerializeNumber::GetNumType()
{
	return numType;
}

SerializeString::SerializeString() : 
	str("")
{
	type = SerializeType::String;
}

SerializeString::SerializeString(std::string_view str) : 
	SerializeString()
{
	this->str = str;
}

void SerializeString::WriteStr(const std::string str)
{
	this->str = str;
}

void SerializeString::ReadStr(std::string& str)
{
	str = this->str;
}

SerializeArray::SerializeArray()
{
	type = SerializeType::Array;
}

void SerializeArray::Append(SerializeValue* elem)
{
	elems.emplace_back(elem);
}

size_t SerializeArray::Count()
{
	return elems.size();
}

SerializeValue* SerializeArray::At(size_t index)
{
	if (index > elems.size() - 1)
	{
		throw;
		return nullptr;
	}
	return elems[index];
}

SerializeBool::SerializeBool() : 
	b(false)
{
	this->type = SerializeType::Bool;
}

SerializeBool::SerializeBool(bool b) :
	SerializeBool()
{
	this->b = b;
}

void SerializeBool::WriteBool(bool b)
{
	this->b = b;
}

void SerializeBool::ReadBool(bool& b)
{
	b = this->b;
}

SerializeObjectIter::SerializeObjectIter(SerializeObject* obj) : 
	obj(obj)
{
	Reset();
}

void SerializeObjectIter::Reset()
{
	curr = obj->members.begin();
}

bool SerializeObjectIter::HasNext()
{
	return curr != obj->members.end();
}

std::pair<std::string, SerializeValue*> SerializeObjectIter::GetNext()
{
	if (curr == obj->members.end())
	{
		throw;
	}

	auto res = std::make_pair(curr->first, curr->second);
	curr++;
	return res;
}

void JsonSerializer::ToFile(SerializeObject* obj, std::string_view filename)
{
	jdoc.SetObject();
	auto& jalloc = jdoc.GetAllocator();

	SerializeObjectIter iter(obj);
	while (iter.HasNext())
	{
		auto next = iter.GetNext();
		Json::Value jval;
		SerializeSub(jval, next.second);
		jdoc.AddMember(Json::Value(next.first.c_str(), jalloc), jval, jalloc);
	}

	Json::StringBuffer buffer;
	Json::PrettyWriter<Json::StringBuffer> writer(buffer);
	jdoc.Accept(writer);

	std::cout << "Serialized JSON: " << buffer.GetString() << std::endl;
}

SerializeObject* JsonSerializer::FromFile(std::string_view filename)
{
	return nullptr;
}

void JsonSerializer::SerializeSub(Json::Value& jValue, SerializeValue* sValue)
{
	switch (sValue->GetType())
	{
	case SerializeType::Bool:
	{
		bool b;
		SerializeCast::To<SerializeBool>(sValue)->ReadBool(b);
		jValue.SetBool(b);
		break;
	}
	case SerializeType::Number:
	{
		auto sNum = SerializeCast::To<SerializeNumber>(sValue);
		auto sNumType = sNum->GetNumType();
		if (sNumType == typeid(float_t))
		{
			float f;
			sNum->ReadNum(f);
			jValue.SetFloat(f);
		}
		else if (sNumType == typeid(double_t))
		{
			double d;
			sNum->ReadNum(d);
			jValue.SetDouble(d);
		}
		else if (sNumType == typeid(int16_t ))
		{
			int16_t i16;
			sNum->ReadNum(i16);
			jValue.SetInt(i16);
		}
		else if (sNumType == typeid(int32_t))
		{
			int32_t i32;
			sNum->ReadNum(i32);
			jValue.SetInt(i32);
		}
		else if (sNumType == typeid(int64_t))
		{
			int64_t i64;
			sNum->ReadNum(i64);
			jValue.SetInt64(i64);
		}
		else if (sNumType == typeid(uint16_t) || sNumType == typeid(uint32_t) || sNumType == typeid(uint64_t))
		{
			uint64_t ui64;
			sNum->ReadNum(ui64);
			jValue.SetUint64(ui64);
		}
		break;
	}
	case SerializeType::String:
	{
		std::string s;
		SerializeCast::To<SerializeString>(sValue)->ReadStr(s);
		jValue.SetString(s.c_str(), jdoc.GetAllocator());
		break;
	}
	case SerializeType::Array:
	{
		jValue.SetArray();

		auto sArr = SerializeCast::To<SerializeArray>(sValue);
		auto& jalloc = jdoc.GetAllocator();
		for (size_t i = 0; i < sArr->Count(); i++)
		{
			Json::Value subJValue;
			SerializeSub(subJValue, sArr->At(i));
			jValue.PushBack(subJValue, jalloc);
		}
		break;
	}
	case SerializeType::Object:
	{
		auto& jalloc = jdoc.GetAllocator();
		jValue.SetObject();

		auto sObj = SerializeCast::To<SerializeObject>(sValue);
		SerializeObjectIter iter(sObj);
		while (iter.HasNext())
		{
			auto next = iter.GetNext();
			Json::Value subJValue;
			SerializeSub(subJValue, next.second);
			jValue.AddMember(Json::Value(next.first.c_str(), jalloc), subJValue, jalloc);
		}
		break;
	}
	default:
		break;
	}
}
