#include "Utility.h"

#include <fstream>
#include <filesystem>

std::string Mrk::Utility::GBKToUTF8(std::string_view gbkStr)
{
	int wcharSize = MultiByteToWideChar(CP_ACP, 0, gbkStr.data(), -1, NULL, 0);
	std::wstring wstr(wcharSize, 0);
	MultiByteToWideChar(CP_ACP, 0, gbkStr.data(), -1, &wstr[0], wcharSize);

	int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string utf8Str(utf8Size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], utf8Size, NULL, NULL);

	return utf8Str;
}

std::string Mrk::Utility::UTF8ToGBK(std::string_view utf8Str)
{
	int wideCharSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), -1, NULL, 0);
	std::wstring wideStr(wideCharSize, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), -1, &wideStr[0], wideCharSize);

	int gbkSize = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string gbkStr(gbkSize, 0);
	WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &gbkStr[0], gbkSize, NULL, NULL);

	return gbkStr;
}

void Mrk::Utility::SaveJson(const Json::Value& json, std::string_view to)
{
	std::filesystem::path toPath = to.data();

	if (!std::filesystem::exists(toPath.parent_path()))
	{
		std::filesystem::create_directories(toPath.parent_path());
	}

	std::ofstream ofstream(toPath);
	if (ofstream.is_open())
	{
		Json::StringBuffer buf;
		Json::PrettyWriter writer(buf);

		json.Accept(writer);

		ofstream << buf.GetString() << std::endl;
		ofstream.close();
	}
}
