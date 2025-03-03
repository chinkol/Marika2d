#pragma once

#include "Core/IDGenerater/IDGenerater.h"
#include "Core/Object/Object.h"
#include "Core/Reflect/Reflect.h"

#include "Common/Singleton/Singleton.h"
#include "Common/Memory/MemCtrlSystem.h"

#include <map>
#include <unordered_map>
#include <stack>

#ifndef MRK_GAMEOBJECT_CONTENT
#define MRK_GAMEOBJECT_CONTENT(x)																										\
MRK_OBJECT(x)																															\
static inline bool _mrk_macro_##x##_gameobject_register_ = [](){																		\
	Mrk::GameObjectFactory::RegisterGameObject<x>(#x);																					\
	return true;																														\
	}();																																\
virtual inline Json::Value ToJson(Mrk::JsonAllocator& alloctor) override {																\
	Json::Value json = Mrk::ReflectSys::ToJson(*this, alloctor);																		\
	SerializeGameObject(json, alloctor);																								\
	return json;																														\
}																																		\
virtual inline void FromJson(const Json::Value& json) override {																		\
	Mrk::ReflectSys::FromJson(*this, json);																								\
	DeserializeGameObject(json);																										\
}
#endif // !MRK_GAMEOBJECT_CONTENT

#ifndef MRK_GAMEOBJECT
#define MRK_GAMEOBJECT(x)			\
MRK_GAMEOBJECT_CONTENT(x)			\
RTTR_ENABLE(Mrk::GameObject)		
#endif // !MRK_GAMEOBJECT

namespace Mrk
{
	class GameObject;
	class Component;
	class Transform;

	class GameObjectHut : public Singleton<GameObjectHut>
	{
		MRK_SINGLETON(GameObjectHut)
		friend class GameObjectFactory;
	public:
		static std::shared_ptr<GameObject> GetObejct(uint64_t id);
	private:
		static void AddObject(std::shared_ptr<GameObject> gameObject);
	private:
		std::unordered_map<uint64_t, std::weak_ptr<GameObject>> objects;
	};

	class GameObjectFactory : public Singleton<GameObjectFactory>
	{
		MRK_SINGLETON(GameObjectFactory)
	public:
		template<typename T>
		static void RegisterGameObject(std::string_view classname);
		static std::shared_ptr<GameObject> CreateNew(std::string_view classname);
		template<typename T>
		static std::shared_ptr<GameObject> CreateNew();
		static const std::map<std::string, std::function<std::shared_ptr<GameObject>()>>& GetCreators();
		static const std::vector<std::string>& GetManifest();
	private:
		GameObjectFactory() = default;
		std::map<std::string, std::function<std::shared_ptr<GameObject>()>> creators;
		std::vector<std::string> manifest;
	};

	class GameObject : public Object, public std::enable_shared_from_this<GameObject>
	{
		MRK_GAMEOBJECT_CONTENT(GameObject) RTTR_ENABLE(Object)
	public:
		GameObject();

		ID GetID();
		void SetID_(ID id);
		const std::string& GetName();
		void SetName(const std::string& name);

		std::shared_ptr<GameObject> GetParent();
		const std::vector<std::shared_ptr<GameObject>>& GetChildren();

		template<typename T> void AddComponent();
		void AddComponent(std::string_view name);
		void AddComponent(std::shared_ptr<Component> component);
		template<typename T> void RemoveComponent();
		void RemoveComponent(std::string_view name);
		template<typename T> std::shared_ptr<T> GetComponent();
		std::shared_ptr<Component> GetComponent(std::string_view name);
		void AddChild(std::shared_ptr<GameObject> child);
		void RemoveChild(std::shared_ptr<GameObject> child);
		virtual void Init();
	protected:
		void DeserializeGameObject(const Json::Value& json);
		void SerializeGameObject(Json::Value& json, Mrk::JsonAllocator& alloctor);
	private:
		void DeserializeComponents(const Json::Value& json);
		void DeserializeChildren(const Json::Value& json);
		Json::Value SerializeComponents(Mrk::JsonAllocator& alloctor);
		Json::Value SerializeChildren(Mrk::JsonAllocator& alloctor);
	protected:
		ID id;
		std::string name;
		std::weak_ptr<GameObject> parent;
		std::shared_ptr<Transform> transform;
		std::vector<std::shared_ptr<GameObject>> children;
		std::map<std::string, std::shared_ptr<Component>> components;
	};
}

#include "Core/Component/Component.h"
#include "Core/Component/Transform/Transform.h"

namespace Mrk
{
	template<typename T>
	inline void GameObject::AddComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T Is Not A Component !");

		auto ret = components.find(T::StaticGetClassTypeName().data());
		if (ret == components.end())
		{
			components.emplace(T::StaticGetClassTypeName().data(), [this]() {
				auto com = ComponentFactory::CreateNew<T>();
				com->holder = weak_from_this();

				if constexpr (std::is_same_v<T, Transform>)
				{
					transform = com;
				}

				return com;
				}());
		}
	}

	template<typename T>
	inline void GameObject::RemoveComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T Is Not A Component !");

		if constexpr (std::is_same_v<T, Transform>)
		{
			return; //TODO log
		}
		else
		{
			components.erase(T::StaticGetClassTypeName().data());
		}
	}

	template<typename T>
	inline std::shared_ptr<T> GameObject::GetComponent()
	{
		if constexpr (std::is_same_v<T, Transform>)
		{
			return transform;
		}
		else
		{
			auto ret = components.find(T::StaticGetClassTypeName().data());
			if (ret != components.end())
			{
				return std::dynamic_pointer_cast<T>(ret->second);
			}

			return std::shared_ptr<T>();
		}
	}

	template<typename T>
	inline void GameObjectFactory::RegisterGameObject(std::string_view classname)
	{
		static_assert(std::is_base_of_v<GameObject, T>, "T Is Not A GameObject !");

		auto ret = Instance().creators.find(classname.data());
		if (ret == Instance().creators.end())
		{
			Instance().manifest.push_back(classname.data());
			Instance().creators.emplace(classname.data(), []() {
				return Mrk::MemCtrlSystem::CreateNew<T>();
				});
		}
	}

	template<typename T>
	inline std::shared_ptr<GameObject> GameObjectFactory::CreateNew()
	{
		static_assert(std::is_base_of_v<GameObject, T>, "T Is Not A GameObject !");
		auto obj = Mrk::MemCtrlSystem::CreateNew<T>();
		GameObjectHut::AddObject(obj);
		static std::string name = T::StaticGetClassTypeName().data();
		obj->SetName(name);
		obj->AddComponent<Transform>();
		obj->Init();
		return obj;
	}
}