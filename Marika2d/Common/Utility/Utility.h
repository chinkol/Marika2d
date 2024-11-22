#pragma once

#include <string>
#include <Windows.h>

namespace Mrk
{
	class Utility
	{
	public:
		static std::string GBKToUTF8(std::string_view gbkStr);
		static std::string UFT8ToGBK(std::string_view utf8Str);
	private:
		Utility();
	};
}


