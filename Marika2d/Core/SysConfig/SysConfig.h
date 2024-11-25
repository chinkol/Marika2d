#include "Common/Singleton/Singleton.h"

#include <any>
#include <map>
#include <string>

#ifndef MRK_CONFIG
#define MRK_CONFIG(type, field, value)												\
private: type field = value;														\
public: static inline const type& Get##field() { return Instance().field; }			\
public: static inline void Set##field(const type& val) { Instance().field = val; }	
#endif // !MRK_CONFIG

namespace Mrk
{
	class SysConfig : public Singleton<SysConfig>
	{
		MRK_SINGLETON(SysConfig)
		MRK_CONFIG(std::string, ProjectPath, "");
		MRK_CONFIG(std::string, AssetPath, "");
		MRK_CONFIG(std::string, MainSceneFile, "");

	private:
		SysConfig() = default;
	};
}