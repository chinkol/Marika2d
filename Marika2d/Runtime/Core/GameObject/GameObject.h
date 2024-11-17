#pragma once

#include "Core/Object/Object.h"
#include "Common/Serialize/SerializeSystem.h"
#include "Common/Singleton/Singleton.h"
#include "Common/Memory/MemCtrlSystem.h"

#include <map>

#ifndef MRK_GAMEOBJECT
#define MRK_GAMEOBJECT(x) MRK_OBJECT(x) MRK_SERIALIZABLE(x) private: static inline bool _mrk_macro_##x##_gameobject_register_ = [](){Mrk::GameObjectFactory::RegisterGameObject<x>(#x); return true; }();
#endif // !MRK_GAMEOBJECT

namespace Mrk
{
	class GameObject;
	class Component;

	class GameObjectFactory : public Singleton<GameObjectFactory>
	{
		MRK_SINGLETON(GameObjectFactory)
	public:
		template<typename T>
		static void RegisterGameObject(std::string_view classname);
		static std::shared_ptr<GameObject> CreateNew(std::string_view classname);
		template<typename T> std::shared_ptr<GameObject> CreateNew();
	private:
		GameObjectFactory() = default;
		std::map<std::string, std::function<std::shared_ptr<GameObject>()>> creators;
	};

	class GameObject : public Object, public ISerializable, public std::enable_shared_from_this<GameObject>
	{
		MRK_GAMEOBJECT(GameObject)
		friend class GameObjectOperate;
	public:
		virtual void DeSerialize(const Json::Value& jobj);
		virtual void Serialize(Json::Value& jobj, JsonAlloc& jalloc) const;
	private:
		std::string name;
		std::weak_ptr<GameObject> parent;
		std::vector<std::shared_ptr<Object>> children;
		std::map<std::string, std::shared_ptr<Component>> components;
	};

	//I dont konw why I want to add this class instead of write these funcs into 'gameobject' class, maybe it makes 'gameobject' looks more clearly ?
	class GameObjectOperate : public Singleton<GameObjectOperate>
	{
	public:
		static void AttachChild(const std::shared_ptr<GameObject>& child, const std::shared_ptr<GameObject>& parent);
		static void DetachChild(const std::shared_ptr<GameObject>& child, const std::shared_ptr<GameObject>& parent);
		static void AttachComponent(const std::shared_ptr<Component>& component, const std::shared_ptr<GameObject>& holder);
		static void AttachComponent(std::string_view comName, const std::shared_ptr<GameObject>& holder);
		template<typename T> static void AttachComponent(const std::shared_ptr<GameObject>& holder);
		static void DetachComponent(const std::shared_ptr<Component>& component, const std::shared_ptr<GameObject>& holder);
		static void DetachComponent(std::string_view comName, const std::shared_ptr<GameObject>& holder);
		template<typename T> static void DetachComponent(const std::shared_ptr<GameObject>& holder);
		static std::shared_ptr<Component> GetComponent(std::string_view comName, const std::shared_ptr<GameObject>& holder);
		template<typename T> static std::shared_ptr<Component> GetComponent(const std::shared_ptr<GameObject>& holder);
	private:
		GameObjectOperate() = default;
	};
}

#include "Core/Component/Component.h"

namespace Mrk
{
	template<typename T>
	inline void GameObjectFactory::RegisterGameObject(std::string_view classname)
	{
		static_assert(std::is_base_of_v<GameObject, T>, "T Is Not A GameObject !");
		auto ret = Instance().creators.try_emplace(classname.data(), []() {
			return Mrk::MemCtrlSystem::CreateNew<T>();
			});
		if (!ret.second)
		{
			//log
		}
	}

	template<typename T>
	inline std::shared_ptr<GameObject> GameObjectFactory::CreateNew()
	{
		static_assert(std::is_base_of_v<GameObject, T>, "T Is Not A GameObject !");
		return Mrk::MemCtrlSystem::CreateNew<T>();
	}

	template<typename T>
	inline void GameObjectOperate::AttachComponent(const std::shared_ptr<GameObject>& holder)
	{
		static_assert(std::is_base_of_v<Component, T>, "T Is Not A Component !");
		AttachComponent(Mrk::MemCtrlSystem::CreateNew<T>(), holder);
	}

	template<typename T>
	inline void GameObjectOperate::DetachComponent(const std::shared_ptr<GameObject>& holder)
	{
		static_assert(std::is_base_of_v<Component, T>, "T Is Not A Component !");
		assert(holder);
		auto ret = holder->components.find(T::GetStaticClassName().data());
		if (ret != holder->components.end())
		{
			holder->components.erase(ret);
			ret->second->holder = std::weak_ptr<GameObject>();
		}
		else
		{
			//log : not found
		}
	}
	template<typename T>
	inline std::shared_ptr<Component> GameObjectOperate::GetComponent(const std::shared_ptr<GameObject>& holder)
	{
		return GetComponent(T::GetStaticClassName(), holder);
	}
}


