#pragma once

#include "Third/rapidjson/document.h"
#include "Third/rapidjson/stringbuffer.h"
#include "Third/rapidjson/prettywriter.h"

#include <filesystem>
#include <string>
#include <Windows.h>

namespace Mrk
{
	class Utility
	{
	public:
		static std::string GBKToUTF8(std::string_view gbkStr);
		static std::string UTF8ToGBK(std::string_view utf8Str);
		static void SaveJson(const Json::Value& json, std::string_view to);
		static Json::Document ReadJson(const std::filesystem::path& path);
	private:
		Utility();
	};
}


