#include "Reflect.h" =================================================================

#include <type_traits>

// to json

namespace
{
	Json::Value RecurVariantToJson(const rttr::variant& variant, Mrk::JsonAllocator& allocator);
	Json::Value RecurObjectToJson(rttr::instance obj, Mrk::JsonAllocator& allocator);
	Json::Value RecurArithmeticToJson(const rttr::variant& atomic, Mrk::JsonAllocator& allocator);
	Json::Value RecurEnumToJson(const rttr::variant& enumeration, Mrk::JsonAllocator& allocator);
	Json::Value RecurStringToJson(const rttr::variant& str, Mrk::JsonAllocator& allocator);
	Json::Value RecurSeqToJson(const rttr::variant& seq, Mrk::JsonAllocator& allocator);
	Json::Value RecurAssToJson(const rttr::variant& ass, Mrk::JsonAllocator& allocator);

	Json::Value RecurSeqToJson(const rttr::variant& seq, Mrk::JsonAllocator& allocator)
	{
		Json::Value jseq(Json::ArrayType);

		auto seqView = seq.create_sequential_view();
		for (auto& item : seqView)
		{
			jseq.PushBack(RecurVariantToJson(item, allocator), allocator);
		}

		return jseq;
	}

	Json::Value RecurAssToJson(const rttr::variant& ass, Mrk::JsonAllocator& allocator)
	{
		Json::Value jass(Json::ObjectType);

		auto assView = ass.create_associative_view();
		for (auto& item : assView)
		{
			jass.AddMember(RecurVariantToJson(item.first, allocator), RecurVariantToJson(item.second, allocator), allocator);
		}

		return jass;
	}

	Json::Value RecurStringToJson(const rttr::variant& str, Mrk::JsonAllocator& allocator)
	{
		return Json::Value(str.to_string(), allocator);
	}

	Json::Value RecurEnumToJson(const rttr::variant& enumeration, Mrk::JsonAllocator& allocator)
	{
		bool ok = false;
		auto result = enumeration.to_string(&ok);
		if (ok)
		{
			return Json::Value(enumeration.to_string(), allocator);
		}
		else
		{
			auto value = enumeration.to_uint32(&ok);
			if (ok)
			{
				return Json::Value(value);
			}
			else
			{
				return Json::Value((uint32_t)0);
			}
		}
	}

	Json::Value RecurVariantToJson(const rttr::variant& variant, Mrk::JsonAllocator& allocator)
	{
		auto type = variant.get_type();
		auto wrapedType = type.is_wrapper() ? type.get_wrapped_type() : type;

		if (type == wrapedType)	// not wrapper
		{
			if (type.is_arithmetic())
			{
				return RecurArithmeticToJson(variant, allocator);
			}
			else if (type.is_enumeration())
			{
				return RecurEnumToJson(variant, allocator);
			}
			else if (type == rttr::type::get<std::string>())
			{
				return RecurStringToJson(variant, allocator);
			}
			else if (type.is_associative_container())
			{
				return RecurAssToJson(variant, allocator);
			}
			else if (type.is_sequential_container())
			{
				return RecurSeqToJson(variant, allocator);
			}
			else // object
			{
				return RecurObjectToJson(variant, allocator);
			}
		}
		else // wrapper type
		{
			auto unwrap = variant.extract_wrapped_value();
			return RecurVariantToJson(unwrap, allocator);
		}
	}

	Json::Value RecurArithmeticToJson(const rttr::variant& arithmetic, Mrk::JsonAllocator& allocator)
	{
		Json::Value jarithmetic(Json::NumberType);

		auto type = arithmetic.get_type();
		if (type.is_arithmetic())
		{
			if (type == rttr::type::get<bool>())
				jarithmetic = arithmetic.to_bool();
			else if (type == rttr::type::get<char>())
				jarithmetic = arithmetic.to_uint8();
			else if (type == rttr::type::get<int8_t>())
				jarithmetic = arithmetic.to_int8();
			else if (type == rttr::type::get<int16_t>())
				jarithmetic = arithmetic.to_int16();
			else if (type == rttr::type::get<int32_t>())
				jarithmetic = arithmetic.to_int32();
			else if (type == rttr::type::get<int64_t>())
				jarithmetic = arithmetic.to_int64();
			else if (type == rttr::type::get<uint8_t>())
				jarithmetic = arithmetic.to_uint8();
			else if (type == rttr::type::get<uint16_t>())
				jarithmetic = arithmetic.to_uint16();
			else if (type == rttr::type::get<uint32_t>())
				jarithmetic = arithmetic.to_uint32();
			else if (type == rttr::type::get<uint64_t>())
				jarithmetic = arithmetic.to_uint64();
			else if (type == rttr::type::get<float>())
				jarithmetic = arithmetic.to_double();
			else if (type == rttr::type::get<double>())
				jarithmetic = arithmetic.to_double();
		}

		return jarithmetic;
	}

	Json::Value RecurObjectToJson(rttr::instance obj, Mrk::JsonAllocator& allocator)
	{
		Json::Value jobj(Json::ObjectType);

		if (obj.is_valid())
		{
			auto props = obj.get_derived_type().get_properties();
			for (auto& prop : props)
			{
				Json::Value jprop;

				auto value = prop.get_value(obj);
				if (value)
				{
					jprop = RecurVariantToJson(value, allocator);
				}

				jobj.AddMember(Json::StringRef(prop.get_name().data()), jprop, allocator);
			}
		}

		return jobj;
	}
}

Json::Value Mrk::ReflectSys::ToJson(rttr::instance obj, Mrk::JsonAllocator& alloctor)
{
	return RecurObjectToJson(obj, alloctor);
}

// from json

namespace
{
	void RecurObjectFromJson(rttr::instance obj, const Json::Value& jobj);
	void RecurVariantFromJson(rttr::variant& variant, const Json::Value& jvariant);
	void RecurArithmeticFromJson(rttr::variant& arithmetic, const Json::Value& jarithmetic);
	void RecurEnumFromJson(rttr::variant& _enum, const Json::Value& jenum);
	void RecurStringFromJson(rttr::variant& str, const Json::Value& jstr);
	void RecurSeqFromJson(rttr::variant& seq, const Json::Value& jseq);
	void RecurAssFromJson(rttr::variant& ass, const Json::Value& jass);

	void RecurAssFromJson(rttr::variant& ass, const Json::Value& jass)
	{
		auto view = ass.create_associative_view();
		auto jdic = jass.GetObject();

		auto keyType = view.get_key_type();
		auto valueType = view.get_value_type();

		for (auto& jitem : jdic)
		{
			auto key = keyType.create();
			auto value = valueType.create();

			RecurVariantFromJson(key, jitem.name);
			RecurVariantFromJson(value, jitem.value);

			if (key.is_valid() && value.is_valid())
			{
				view.insert(key, value);
			}
		}
	}

	void RecurSeqFromJson(rttr::variant& seq, const Json::Value& jseq)
	{
		auto view = seq.create_sequential_view();
		auto jarr = jseq.GetArray();

		view.set_size(jarr.Size());

		int index = 0;
		for (auto& jitem : jarr)
		{
			auto value = view.get_value(index);
			RecurVariantFromJson(value, jitem);
			if (value.is_valid())
			{
				view.set_value(index, value);
				index++;
			}
		}
	}

	void RecurStringFromJson(rttr::variant& str, const Json::Value& jstr)
	{
		if (jstr.IsString())
		{
			str = std::string(jstr.GetString());
		}
	}

	void RecurEnumFromJson(rttr::variant& _enum, const Json::Value& jenum)
	{
		if (jenum.IsInt())
		{
			_enum = jenum.GetInt();
		}
		else if (jenum.IsString())
		{
			_enum = jenum.GetString();
		}
	}

	void RecurArithmeticFromJson(rttr::variant& arithmetic, const Json::Value& jarithmetic)
	{
		rttr::variant trans;

		switch (jarithmetic.GetType())
		{
		case Json::FalseType:
		case Json::TrueType:
			trans = jarithmetic.GetBool();
			break;
		case Json::NumberType:
		{
			if (jarithmetic.IsInt())
				trans = jarithmetic.GetInt();
			else if (jarithmetic.IsDouble())
				trans = jarithmetic.GetDouble();
			else if (jarithmetic.IsUint())
				trans = jarithmetic.GetUint();
			else if (jarithmetic.IsInt64())
				trans = jarithmetic.GetInt64();
			else if (jarithmetic.IsUint64())
				trans = jarithmetic.GetUint64();
			break;
		}
		}

		if (trans.convert(arithmetic.get_type()))
		{
			arithmetic = trans;
		}
	}

	void RecurVariantFromJson(rttr::variant& variant, const Json::Value& jvariant)
	{
		auto type = variant.get_type();
		if (type.is_arithmetic())
		{
			RecurArithmeticFromJson(variant, jvariant);
		}
		else if (type.is_enumeration())
		{
			RecurEnumFromJson(variant, jvariant);
		}
		else if (type == rttr::type::get<std::string>())
		{
			RecurStringFromJson(variant, jvariant);
		}
		else if (type.is_sequential_container())
		{
			RecurSeqFromJson(variant, jvariant);
		}
		else if (type.is_associative_container())
		{
			RecurAssFromJson(variant, jvariant);
		}
		else
		{
			RecurObjectFromJson(variant, jvariant);
		}
	}

	void RecurObjectFromJson(rttr::instance obj, const Json::Value& jobj)
	{
		if (obj.is_valid())
		{
			auto props = obj.get_derived_type().get_properties();
			for (auto& prop : props)
			{
				auto jprop = jobj.FindMember(prop.get_name().data());
				if (jprop != jobj.MemberEnd())
				{
					auto value = prop.get_value(obj);
					RecurVariantFromJson(value, jprop->value);
					if (value.is_valid())
					{
						auto type = prop.get_type();
						prop.set_value(obj, value);
					}
				}
			}
		}
	}
}

void Mrk::ReflectSys::FromJson(rttr::instance obj, const Json::Value& jobj)
{
	if (obj.is_valid())
	{
		RecurObjectFromJson(obj, jobj);
	}
}