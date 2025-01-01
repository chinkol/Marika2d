#include "Reflect.h"

#include <string_view>

namespace
{
	void WriteVariant(Json::PrettyWriter<Json::StringBuffer>& writer, const rttr::variant& variant);
	void WriteSeqContainer(Json::PrettyWriter<Json::StringBuffer>& writer, const rttr::variant& variant);
	void WriteAssContainer(Json::PrettyWriter<Json::StringBuffer>& writer, const rttr::variant& variant);
	void WriteArithmetic(Json::PrettyWriter<Json::StringBuffer>& writer, const rttr::variant& variant, const rttr::type& type);
	void WriteEnumeration(Json::PrettyWriter<Json::StringBuffer>& writer, const rttr::variant& variant);
	void WriteObject(Json::PrettyWriter<Json::StringBuffer>& writer, rttr::instance obj);

	void WriteSeqContainer(Json::PrettyWriter<Json::StringBuffer>& writer, const rttr::variant& variant)
	{
		writer.StartArray();

		auto seqView = variant.create_sequential_view();
		for (auto& item : seqView)
		{
			WriteVariant(writer, item);
		}

		writer.EndArray();
	}

	void WriteAssContainer(Json::PrettyWriter<Json::StringBuffer>& writer, const rttr::variant& variant)
	{
		auto assView = variant.create_associative_view();
		if (assView.is_key_only_type())
		{
			writer.StartArray();
			
			for (auto& item : assView)
			{
				WriteVariant(writer, item.first);
			}

			writer.EndArray();
		}
		else
		{
			writer.StartObject();

			for (auto& item : assView)
			{
				auto name = item.second.get_type().get_raw_type().get_name();
				WriteVariant(writer, item.first);
				WriteVariant(writer, item.second);
			}

			writer.EndObject();
		}

	}

	void WriteArithmetic(Json::PrettyWriter<Json::StringBuffer>& writer, const rttr::variant& variant, const rttr::type& type)
	{
		if (type == rttr::type::get<bool>())
			writer.Bool(variant.to_bool());
		else if (type == rttr::type::get<char>())
			writer.Bool(variant.to_bool());
		else if (type == rttr::type::get<int8_t>())
			writer.Int(variant.to_int8());
		else if (type == rttr::type::get<int16_t>())
			writer.Int(variant.to_int16());
		else if (type == rttr::type::get<int32_t>())
			writer.Int(variant.to_int32());
		else if (type == rttr::type::get<int64_t>())
			writer.Int64(variant.to_int64());
		else if (type == rttr::type::get<uint8_t>())
			writer.Uint(variant.to_uint8());
		else if (type == rttr::type::get<uint16_t>())
			writer.Uint(variant.to_uint16());
		else if (type == rttr::type::get<uint32_t>())
			writer.Uint(variant.to_uint32());
		else if (type == rttr::type::get<uint64_t>())
			writer.Uint64(variant.to_uint64());
		else if (type == rttr::type::get<float>())
			writer.Double(variant.to_double());
		else if (type == rttr::type::get<double>())
			writer.Double(variant.to_double());
	}

	void WriteEnumeration(Json::PrettyWriter<Json::StringBuffer> & writer, const rttr::variant& variant)
	{
		bool ok = false;
		auto result = variant.to_string(&ok);
		if (ok)
		{
			writer.String(variant.to_string());
		}
		else
		{
			auto value = variant.to_uint32(&ok);
			if (ok)
			{
				writer.Uint64(value);
			}
			else
			{
				writer.Null();
			}
		}
	}

	void WriteObject(Json::PrettyWriter<Json::StringBuffer>& writer, rttr::instance obj)
	{
		static const rttr::string_view classKey("MrkClassKey");
		static const rttr::string_view classValue("MrkClassValue");

		writer.StartObject();

		writer.String(classKey.data());
		writer.String(obj.get_derived_type().get_raw_type().get_name().data());

		writer.String(classValue.data());
		{
			writer.StartObject();

			auto props = obj.get_derived_type().get_properties();
			for (auto& prop : props)
			{
				auto value = prop.get_value(obj);
				if (value)
				{
					auto name = prop.get_name();
					writer.String(name.data(), static_cast<Json::SizeType>(name.length()), false);
					WriteVariant(writer, value);
				}
			}

			writer.EndObject();
		}
		
		writer.EndObject();
	}

	void WriteVariant(Json::PrettyWriter<Json::StringBuffer>& writer, const rttr::variant& variant)
	{
		auto valueType = variant.get_type().get_raw_type();
		auto wrapType = valueType.is_wrapper() ? valueType.get_wrapped_type() : valueType;

		if (valueType == wrapType)	// not wrapper
		{
			if (valueType.is_arithmetic())
			{
				WriteArithmetic(writer, variant, valueType);
			}
			else if (valueType.is_enumeration())
			{
				WriteEnumeration(writer, variant);
			}
			else if (valueType == rttr::type::get<std::string>())
			{
				writer.String(variant.to_string());
			}
			else if (valueType.is_associative_container())
			{
				WriteAssContainer(writer, variant);
			}
			else if (valueType.is_sequential_container())
			{
				WriteSeqContainer(writer, variant);
			}
			else // object
			{
				WriteObject(writer, variant);
			}
		}
		else // wrapper type
		{
			auto unwrappType = variant.extract_wrapped_value();
			WriteVariant(writer, unwrappType);
		}
	}

	void ReadVariant(rttr::variant variant, Json::Value& jvariant);

	rttr::variant ReadBasicType(Json::Value& jvariant)
	{
		switch (jvariant.GetType())
		{
		case Json::StringType:
		{
			return std::string(jvariant.GetString());
			break;
		}
		case Json::NullType:     
			break;
		case Json::FalseType:
		case Json::TrueType:
		{
			return jvariant.GetBool();
			break;
		}
		case Json::NumberType:
		{
			if (jvariant.IsInt())
				return jvariant.GetInt();
			else if (jvariant.IsDouble())
				return jvariant.GetDouble();
			else if (jvariant.IsUint())
				return jvariant.GetUint();
			else if (jvariant.IsInt64())
				return jvariant.GetInt64();
			else if (jvariant.IsUint64())
				return jvariant.GetUint64();
			break;
		}
		// we handle only the basic types here
		case Json::ObjectType:
		case Json::ArrayType: 
			return rttr::variant();
		}

		return rttr::variant();
	}

	rttr::variant ReadObject(Json::Value& jobj)
	{
		assert(jobj.IsObject());

		auto jclassKey = jobj.FindMember("MrkClassKey");
		auto jclassValue = jobj.FindMember("MrkClassValue");

		if (jclassKey != jobj.MemberEnd() && jclassValue != jobj.MemberEnd() && jclassKey->value.IsString() && jclassValue->value.IsObject())
		{
			auto n = jclassKey->value.GetString();
			auto type = rttr::type::get_by_name(jclassKey->value.GetString());
			if (auto constructor = type.get_constructor())
			{
				auto obj = constructor.invoke();
				auto& jobj = jclassValue->value;

				auto props = type.get_properties();
				for (auto& prop : props)
				{
					auto jprop = jobj.FindMember(prop.get_name().data());
					if (jprop != jobj.MemberEnd())
					{
						ReadVariant(prop.get_value(obj), jprop->value);
					}
				}

				return obj;
			}
		}

		return rttr::variant();
	}

	void ReadAssContainer(rttr::variant ass, Json::Value& jass)
	{
		
	}

	void ReadSeqContainer(rttr::variant seq, Json::Value& jseq)
	{

	}

	void ReadVariant(rttr::variant variant, Json::Value& jvariant)
	{
		auto type = variant.get_type();
		if (jvariant.IsObject())
		{
			if (variant.is_associative_container())
			{
				ReadAssContainer(variant, jvariant);
			}
			else
			{
				variant = ReadObject(jvariant);
			}
		}
		else if (variant.is_sequential_container() && jvariant.IsArray())
		{
			ReadSeqContainer(variant, jvariant);
		}
		else
		{
			auto basic = ReadBasicType(jvariant);
			if (basic.convert(variant.get_type()))
			{
				variant = basic;
			}
		}
	}
}

std::string MrkNew::ReflectSystem::ToJson(rttr::instance obj)
{
	if (!obj.is_valid())
		return std::string();

	Json::StringBuffer buffer;
	Json::PrettyWriter writer(buffer);

	WriteObject(writer, obj);

	return buffer.GetString();
}

rttr::variant MrkNew::ReflectSystem::FromJson(const std::string& json)
{
	Json::Document jdoc;
	if (jdoc.Parse(json.c_str()).HasParseError())
	{
		throw;
	}

	if (jdoc.IsObject())
	{
		return ReadObject(jdoc);
	}

	return rttr::variant();
}
