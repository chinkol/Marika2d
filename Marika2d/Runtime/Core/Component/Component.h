#pragma once

#include "Core/Object/Object.h"
#include "Common/Serialize/SerializeSystem.h"
#include "Common/Singleton/Singleton.h"
#include "Common/Memory/MemCtrlSystem.h"

#include <map>
#include <string>

#ifndef MRK_COMPONENT
#define MRK_COMPONENT(x) MRK_OBJECT(x) MRK_SERIALIZABLE(x) static inline bool _mrk_macro_##x##_component_register_ = [](){ Mrk::ComponentFactory::RegisterComponent<x>(#x); return true;}();
#endif // !MRK_COMPONENT

namespace Mrk
{
	class Component;
	class GameObject;

	class ComponentFactory : public Singleton<ComponentFactory>
	{
		MRK_SINGLETON(ComponentFactory)
	public:
		template<typename T>
		static void RegisterComponent(std::string_view classname);
		static std::shared_ptr<Component> CreateNew(std::string_view classname);
		template<typename T> static std::shared_ptr<Component> CreateNew();
	private:
		ComponentFactory() = default;
		std::map<std::string, std::function<std::shared_ptr<Component>()>> creators;
	};

	class Component : public Object, public ISerializable, public std::enable_shared_from_this<Component>
	{
		MRK_COMPONENT(Component)
		friend class GameObjectOperate;
	public:
		Component() = default;
		virtual ~Component() = default;
		std::weak_ptr<GameObject> GetHolder();
	protected:
		std::weak_ptr<GameObject> holder;
	};
}

namespace Mrk
{
	template<typename T>
	inline void ComponentFactory::RegisterComponent(std::string_view classname)
	{
		auto ret = Instance().creators.try_emplace(classname.data(), []() {
			return MemCtrlSystem::CreateNew<Component>();
			});
		if (!ret.second)
		{
			//log
		}
	}

	template<typename T>
	inline std::shared_ptr<Component> ComponentFactory::CreateNew()
	{
		static_assert(std::is_base_of_v<Component, T>, "T Is Not A Component !");
		return CreateNew(T::GetStaticClassName());
	}
}


