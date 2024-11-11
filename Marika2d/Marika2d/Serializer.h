#pragma once
#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <typeindex>

#define RAPIDJSON_HAS_STDSTRING 1
#include "Third/rapidjson/document.h"

class SerializeObject;
class SerializeValue;

class ISerializer
{
public:
	virtual void ToFile(SerializeObject* field, std::string_view filename) = 0;
	virtual SerializeObject* FromFile(std::string_view filename) = 0;
};

class JsonSerializer : ISerializer
{
public:
	virtual void ToFile(SerializeObject* field, std::string_view filename) override;
	virtual SerializeObject* FromFile(std::string_view filename) override;
private:
	void SerializeSub(Json::Value& jValue, SerializeValue* sValue);
	Json::Document jdoc;
};

enum class SerializeType
{
	Bool,
	Number,
	String,
	Array,
	Object,
};

class SerializeValue
{
public:
	virtual ~SerializeValue();
	SerializeType GetType();
protected:
	SerializeType type;
};

class ISerializable;

class SerializeObject : public SerializeValue
{
	friend class SerializeObjectIter;
public:
	SerializeObject();
	SerializeObject(const ISerializable* obj);

	void WriteObj(const ISerializable* obj);
	void ReadObj(ISerializable* obj);
	void AddMember(std::string_view memberName, SerializeValue* member);
	SerializeValue* GetMember(std::string_view memberName);

private:
	std::map<std::string, SerializeValue*> members;
};

class SerializeObjectIter
{
public:
	SerializeObjectIter(SerializeObject* obj);

	void Reset();
	bool HasNext();
	std::pair<std::string, SerializeValue*> GetNext();

private:
	SerializeObject* obj;
	std::map<std::string, SerializeValue*>::iterator curr;
};

class SerializeArray : public SerializeValue
{
public:
	SerializeArray();

	size_t Count();
	void Append(SerializeValue* elem);
	SerializeValue* At(size_t index);

private:
	std::vector<SerializeValue*> elems;
};

class SerializeNumber : public SerializeValue
{
public:
	SerializeNumber();
	~SerializeNumber();
	template <typename T> SerializeNumber(T num);

	template <typename T> void WriteNum(T num);
	template <typename T> void ReadNum(T& num);
	std::type_index GetNumType();
private:
	std::type_index numType = typeid(nullptr);
	void* num;
};

class SerializeString : public SerializeValue
{
public:
	SerializeString();
	SerializeString(std::string_view str);

	void WriteStr(const std::string str);
	void ReadStr(std::string& str);

private:
	std::string str;
};

class SerializeBool : public SerializeValue
{
public:
	SerializeBool();
	SerializeBool(bool b);

	void WriteBool(bool b);
	void ReadBool(bool& b);

private:
	bool b;
};

class ISerializable
{
public:
	virtual inline void Serialize(SerializeObject* obj) const {};
	virtual inline void DeSerialize(SerializeObject* obj) {};
};

struct SerializeCast
{
	template<typename T> static T* To(SerializeValue* field);
};

template<typename T>
inline T* SerializeCast::To(SerializeValue* field)
{
	static_assert(std::is_base_of_v<SerializeValue, T>, "Serialize Cast Type Is Not A SerializeField !");

	auto res = dynamic_cast<T*>(field);

	if (!res)
	{
		throw;
	}

	return res;
}

template<typename T>
inline SerializeNumber::SerializeNumber(T num)
{
	static_assert(std::is_arithmetic_v<T>, "Param Type Is Not A Num Type !");

	this->numType = typeid(T);
	this->type = SerializeType::Number;
	this->num = std::malloc(sizeof(T));

	if (this->num)
	{
		*(T*)this->num = num;
	}
	else
	{
		throw;	//malloc failed
	}
}

template<typename T>
inline void SerializeNumber::WriteNum(T num)
{
	this->numType = typeid(T);
	this->num = num;
}

template<typename T>
inline void SerializeNumber::ReadNum(T& num)
{
	static_assert(std::is_arithmetic_v<T>, "Param Type Is Not A Num Type !");
	num = *(T*)this->num;
}
