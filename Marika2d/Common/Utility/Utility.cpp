#include "Utility.h"

#include <fstream>
#include <filesystem>

std::string Mrk::Utility::GBKToUTF8(std::string_view gbkStr)
{
	if (gbkStr.empty()) {
		return "";
	}

	// GBK -> WideChar
	int wcharSize = MultiByteToWideChar(CP_ACP, 0, gbkStr.data(), -1, NULL, 0);
	if (wcharSize <= 0) {
		return ""; // ×ª»»Ê§°Ü
	}
	std::wstring wstr(wcharSize, 0);
	MultiByteToWideChar(CP_ACP, 0, gbkStr.data(), -1, &wstr[0], wcharSize);
	wstr.resize(wcharSize - 1);  // ÒÆ³ýÄ©Î² `\0`

	// WideChar -> UTF-8
	int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	if (utf8Size <= 0) {
		return ""; // ×ª»»Ê§°Ü
	}
	std::string utf8Str(utf8Size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], utf8Size, NULL, NULL);
	utf8Str.resize(utf8Size - 1); // ÒÆ³ýÄ©Î² `\0`

	return utf8Str;
}

std::string Mrk::Utility::UTF8ToGBK(std::string_view utf8Str)
{
	if (utf8Str.empty()) {
		return "";
	}

	// UTF-8 -> WideChar
	int wideCharSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), -1, NULL, 0);
	if (wideCharSize <= 0) {
		return ""; // ×ª»»Ê§°Ü
	}
	std::wstring wideStr(wideCharSize, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), -1, &wideStr[0], wideCharSize);
	wideStr.resize(wideCharSize - 1); // ÒÆ³ýÄ©Î² '\0'

	// WideChar -> GBK
	int gbkSize = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, NULL, 0, NULL, NULL);
	if (gbkSize <= 0) {
		return ""; // ×ª»»Ê§°Ü
	}
	std::string gbkStr(gbkSize, 0);
	WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &gbkStr[0], gbkSize, NULL, NULL);
	gbkStr.resize(gbkSize - 1); // ÒÆ³ýÄ©Î² '\0'

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

Json::Document Mrk::Utility::ReadJson(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
    {
        return Json::Document();
    }

    std::ifstream ifstream(path, std::ios::binary);
    if (!ifstream)
    {
		return Json::Document();
        //throw std::runtime_error("Failed to open file: " + path.string());
    }

    // ¶ÁÈ¡ JSON ÎÄ¼þÄÚÈÝ
    std::ostringstream oss;
    oss << ifstream.rdbuf();
    std::string json = oss.str();

    // ½âÎö JSON
    Json::Document jdoc;
    jdoc.Parse(json.c_str());

    // ¼ì²é½âÎö´íÎó
    if (jdoc.HasParseError())
    {
		return Json::Document();
        //throw std::runtime_error("JSON parse error: " + std::to_string(jdoc.GetParseError()));
    }

    return jdoc;
}
