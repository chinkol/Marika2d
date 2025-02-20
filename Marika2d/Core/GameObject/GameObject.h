#pragma once

#include "Core/Object/Object.h"
#include "Core/Reflect/Reflect.h"

#include "Common/Singleton/Singleton.h"
#include "Common/Memory/MemCtrlSystem.h"

#include <map>

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
	private:
		friend class GameObjectOperate;
	public:
		const std::string& GetName();
		void SetName(const std::string& name);
	protected:
		void DeserializeGameObject(const Json::Value& json);
		void SerializeGameObject(Json::Value& json, Mrk::JsonAllocator& alloctor);
	private:
		void DeserializeComponents(const Json::Value& json);
		void DeserializeChildren(const Json::Value& json);
		Json::Value SerializeComponents(Mrk::JsonAllocator& alloctor);
		Json::Value SerializeChildren(Mrk::JsonAllocator& alloctor);
	protected:
		std::string name;
		std::weak_ptr<GameObject> parent;
		std::vector<std::shared_ptr<GameObject>> children;
		std::vector<std::shared_ptr<Component>> components;
	};

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
		if (Instance().creators.try_emplace(classname.data(), []() {
			return Mrk::MemCtrlSystem::CreateNew<T>();
			}).second)
		{
			Instance().manifest.push_back(classname.data());
		}
		else
		{
			throw; // repeat?
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
		assert(holder);

		AttachComponent(Mrk::MemCtrlSystem::CreateNew<T>(), holder);
	}

	template<typename T>
	inline void GameObjectOperate::DetachComponent(const std::shared_ptr<GameObject>& holder)
	{
		static_assert(std::is_base_of_v<Component, T>, "T Is Not A Component !");
		assert(holder);

		auto& components = holder->components;
		DetachComponent(T::GetStaticClassName(), holder);
	}
	template<typename T>
	inline std::shared_ptr<Component> GameObjectOperate::GetComponent(const std::shared_ptr<GameObject>& holder)
	{
		static_assert(std::is_base_of_v<Component, T>, "T Is Not A Component !");
		assert(holder);

		return GetComponent(T::GetStaticClassName(), holder);
	}
}
