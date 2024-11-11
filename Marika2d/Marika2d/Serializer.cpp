#include "Serializer.h"

#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/stringbuffer.h"

#include <filesystem>

SerializeField::~SerializeField()
{
}

SerializeType SerializeField::GetType()
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

void SerializeObject::AddMember(std::string_view memberName, SerializeField* member)
{
	if (!members.try_emplace(memberName.data(), member).second)
	{
		throw;
	}
}

SerializeField* SerializeObject::GetMember(std::string_view memberName)
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

void SerializeArray::Append(SerializeField* elem)
{
	elems.emplace_back(elem);
}

size_t SerializeArray::Count()
{
	return elems.size();
}

SerializeField* SerializeArray::At(size_t index)
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
	return curr != obj->members.end();;
}

std::pair<std::string_view, SerializeField*> SerializeObjectIter::GetNext()
{
	if (curr == obj->members.end())
	{
		throw;
	}

	auto& ret = curr;
	curr++;
	return std::make_pair(ret->first, ret->second);
}

void JsonSerializer::ToFile(SerializeObject* obj, std::string_view filename)
{
	Json::Document jdoc;
	jdoc.SetObject();
	auto& jalloc = jdoc.GetAllocator();

	SerializeObjectIter iter(obj);
	while (iter.HasNext())
	{
		auto next = iter.GetNext();
		Json::Value val;
		SerializeSub(val, next.second);
		std::string j = "sasd";
		jdoc.AddMember(Json::GenericStringRef(next.first.data()), val, jalloc);
	}
}

SerializeObject* JsonSerializer::FromFile(std::string_view filename)
{
	return nullptr;
}

void JsonSerializer::SerializeSub(Json::Value& value, SerializeField* sub)
{
	Json::Value subValue;

	switch (sub->GetType())
	{
	case SerializeType::Bool:
		bool b;
		SerializeCast::To<SerializeBool>(sub)->ReadBool(b);
		subValue.SetBool(b);
		break;
	case SerializeType::Number:
		break;
	case SerializeType::String:
		break;
	case SerializeType::Array:
		break;
	case SerializeType::Object:
		break;
	default:
		break;
	}
}
