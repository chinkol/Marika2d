#include "SerializeSystem.h"

#include "../Log/LogSystem.h"

#include <assert.h>

Mrk::SerializeManager& Mrk::SerializeManager::GetInstance()
{
	static SerializeManager instance;
	return instance;
}

Mrk::ISerializable* Mrk::SerializeManager::Create(std::string_view className)
{
	auto ret = creaters.find(className.data());
	if (ret != creaters.end())
	{
		return ret->second();
	}
	else
	{
		LogSystem::Log(LogLevel::Error, __FILE__, __FUNCTION__, std::format("Class [{}] Not Be Registed !", className));
		throw std::runtime_error(std::format("Class [{}] Not Be Registed !", className));
	}
}

Mrk::SerializeStream::~SerializeStream()
{
	for (auto& [_, field] : fields)
	{
		delete field;
		field = nullptr;
	}
}

std::map<std::string, Mrk::SerializeFieldBase*>& Mrk::SerializeStream::GetFields()
{
	return fields;
}

Mrk::OutSerializeStream::OutSerializeStream(SerializeStream* stream) : 
	stream(stream)
{
	assert(stream);
}

Mrk::InSerializeStream::InSerializeStream(SerializeStream* stream) : 
	stream(stream)
{
	assert(stream);
}
