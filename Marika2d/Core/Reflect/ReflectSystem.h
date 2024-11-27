#include "Common//Memory/MemCtrlSystem.h"
#include "Common/Singleton/Singleton.h"
#include "Core/Object/Object.h"

#include <functional>
#include <typeindex>
#include <stdint.h>
#include <string>
#include <map>
#include <any>

namespace Mrk
{
	class FieldInfoBase
	{
	public:
		template<typename T, typename V> void SetValue(T* obj, V newVal)
		{
			if (!obj)
			{
				std::remove_cvref_t<V> val = newVal;
				VirSetValue(obj, val);
			}
		}
	protected:
		virtual void VirSetValue(void* obj, std::any newVal) = 0;
	private:
		uint16_t offset;
		uint16_t size;
	};

	template<typename T, typename F, auto Get, auto Set>
	class FieldInfo : public FieldInfoBase
	{
		using Getter = decltype(Get);
		using Setter = decltype(Set);
	public:
		FieldInfo(Getter getter, Setter setter) : 
			getter(getter),
			setter(setter)
		{

		}
	protected:
		virtual void VirSetValue(void* obj, std::any newVal) override
		{
			auto val = std::any_cast<F>(newVal);
			((T*)obj->*setter)(val);
		}
	private:
		Getter getter;
		Setter setter;
	};

	class MethodInfoBase
	{
	protected:
		intptr_t add;
	};

	template<typename T>
	class MethodInfo : public MethodInfoBase
	{

	};

	class ClassInfoBase
	{
	public:
		virtual Object* Instantiate() = 0;
		template<typename T, typename F> void RegisterField(std::string_view fieldname, F T::* field)
		{
			fields.try_emplace(fieldname.data(), )
		}
	protected:
		std::type_index type;
		std::map<std::string, FieldInfoBase*> fields;
		std::map<std::string, MethodInfoBase*> methods;
	};

	template<typename T>
	class ClassInfo : public ClassInfoBase
	{
	public:
		virtual void* Instantiate() override
		{
			return (void*)MemCtrlSystem::CreateNew<T>();
		}
	};

	class ReflectSystem : public Singleton<ReflectSystem>
	{
		MRK_SINGLETON(ReflectSystem)
	public:
		template<typename T> FieldInfoBase* RegisterClass(std::string_view classname)
		{
			auto ret = Instance().classes.try_emplace(classname.data(), new ClassInfo<T>());
			if (ret.second)
			{
				return ret.first->second;
			}
			throw; // repeat ?
		}
	private:
		std::map<std::string, ClassInfoBase*> classes;
	};
}

class ReflectTestClass
{
	int field1;
	float field2;
	double field3;
	std::string field4;
	std::shared_ptr<double> field5;

public:
	void SetField1(int val)
	{
		field1 = val;
	}
	int GetField1()
	{
		return field1;
	}
};

void ReflectTest()
{
	auto fieldInfo = Mrk::FieldInfo<ReflectTestClass, int, ReflectTestClass::SetField1, >(&, ;
}