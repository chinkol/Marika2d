#include "Reflect.h"

std::string MrkNew::ReflectSystem::ToJson(rttr::instance obj)
{
	if (!obj.is_valid())
		return std::string();

	Json::StringBuffer buffer;
	Json::PrettyWriter writer(buffer);

	writer.StartObject();

	rttr::instance ins = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
	auto props = ins.get_derived_type().get_properties();
	for (auto prop : props)
	{
		auto name = prop.get_name();
		writer.String(name.data(), static_cast<Json::SizeType>(name.length()), false);
		/*rttr::variant value = prop.get_value(ins);
		if (value)
		{
		auto name = prop.get_name();
		writer.String(name.data(), static_cast<Json::SizeType>(name.length()), false);
		}*/
	}

	writer.EndObject();

	std::cout << buffer.GetString() << std::endl;

	return "";
}
