#pragma once

#include "Core/Object/Object.h"
#include "Common/Serialize/SerializeSystem.h"
#include "Common/Singleton/Singleton.h"
#include "Common/Memory/MemCtrlSystem.h"

#include <map>
#include <string>

#ifndef MRK_COMPONENT
#define MRK_COMPONENT(x) MRK_OBJECT(x) MRK_SERIALIZABLE(x) template<typename T> friend class ComponentTarit; private: static inline bool _mrk_macro_##x##_component_register_ = [](){ Mrk::ComponentFactory::RegisterComponent<x>(#x); return true;}();
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
		template<typename T> static std::shared_ptr<T> CreateNew();
		static const std::map<std::string, std::function<std::shared_ptr<Component>()>>& GetCreators();
	private:
		ComponentFactory() = default;
		std::map<std::string, std::function<std::shared_ptr<Component>()>> creators;
	};

	template<typename T>
	class ComponentTrait
	{
	private:
		template <typename U> static auto HasStart(U* ptr) -> decltype(ptr->Start(), std::true_type()) {};
		template <typename U> static std::false_type HasStart(...) {};
		template <typename U> static auto HasPreUpdate(U* ptr) -> decltype(ptr->PreUpdate(), std::true_type()) {};
		template <typename U> static std::false_type HasPreUpdate(...) {};
		template <typename U> static auto HasUpdate(U* ptr) -> decltype(ptr->Update(), std::true_type()) {};
		template <typename U> static std::false_type HasUpdate(...) {};
		template <typename U> static auto HasLateUpdate(U* ptr) -> decltype(ptr->LateUpdate(), std::true_type()) {};
		template <typename U> static std::false_type HasLateUpdate(...) {};
		template <typename U> static auto HasFixedUpdate(U* ptr) -> decltype(ptr->FixedUpdate(), std::true_type()) {};
		template <typename U> static std::false_type HasFixedUpdate(...) {};
		template <typename U> static auto HasPreDraw(U* ptr) -> decltype(ptr->PreDraw(), std::true_type()) {};
		template <typename U> static std::false_type HasPreDraw(...) {};
		template <typename U> static auto HasDraw(U* ptr) -> decltype(ptr->Draw(), std::true_type()) {};
		template <typename U> static std::false_type HasDraw(...) {};
		template <typename U> static auto HasLateDraw(U* ptr) -> decltype(ptr->LateDraw(), std::true_type()) {};
		template <typename U> static std::false_type HasLateDraw(...) {};
		template <typename U> static auto HasDispose(U* ptr) -> decltype(ptr->Dispose(), std::true_type()) {};
		template <typename U> static std::false_type HasDispose(...) {};
	public:
		static constexpr bool hasStart = decltype(HasStart<T>(nullptr))::value;
		static constexpr bool hasPreUpdate = decltype(HasPreUpdate<T>(nullptr))::value;
		static constexpr bool hasUpdate = decltype(HasUpdate<T>(nullptr))::value;
		static constexpr bool hasLateUpdate = decltype(HasLateUpdate<T>(nullptr))::value;
		static constexpr bool hasFixedUpdate = decltype(HasFixedUpdate<T>(nullptr))::value;
		static constexpr bool hasPreDraw = decltype(HasPreDraw<T>(nullptr))::value;
		static constexpr bool hasDraw = decltype(HasDraw<T>(nullptr))::value;
		static constexpr bool hasLateDraw = decltype(HasLateDraw<T>(nullptr))::value;
		static constexpr bool hasDispose = decltype(HasDispose<T>(nullptr))::value;
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

	class ComponentCallBack
	{
	public:
		using CallBack = void(Component::*)();
		ComponentCallBack(const std::shared_ptr<Component>& component, CallBack callback);
		void Invoke();
		bool Expired();
	private:
		std::weak_ptr<Component> component;
		CallBack callback;
	};

	class ComponentLoopSystem : public Singleton<ComponentLoopSystem>
	{
		MRK_SINGLETON(ComponentLoopSystem)
	public:
		static void Invoke(std::string_view loopState);
		static void Clean()
		{
			for (auto& [_, state] : Instance().loopStates)
			{
				for (auto& [_, callbacks] : state)
				{
					callbacks->erase(std::remove_if(callbacks->begin(), callbacks->end(), [](ComponentCallBack& callback) { return callback.Expired(); }), callbacks->end());
				}
			}
		}
		template<typename T> static void AddComponent(const std::shared_ptr<T>& component);

	private:
		ComponentLoopSystem() = default;
		template<typename T> static std::vector<ComponentCallBack>* TryEmplaceBatch(std::string_view loopState);
		std::map<std::string, std::map<std::type_index, std::vector<ComponentCallBack>*>> loopStates; // [loopstate, [comtype, {loopfuncs}]];
	};
}

namespace Mrk
{
	template<typename T>
	inline void ComponentLoopSystem::AddComponent(const std::shared_ptr<T>& component)
	{
		auto& loopStates = Instance().loopStates; //debug

		assert(component);
		if constexpr (ComponentTrait<T>::hasPreUpdate)
		{
			static std::vector<ComponentCallBack>* preUpdateBatch = TryEmplaceBatch<T>("PreUpdate");
			preUpdateBatch->emplace_back(component, (ComponentCallBack::CallBack)&T::PreUpdate);
		}
		if constexpr (ComponentTrait<T>::hasUpdate)
		{
			static std::vector<ComponentCallBack>* updateBatch = TryEmplaceBatch<T>("Update");
			updateBatch->emplace_back(component, (ComponentCallBack::CallBack)&T::Update);
		}
		if constexpr (ComponentTrait<T>::hasLateUpdate)
		{
			static std::vector<ComponentCallBack>* lateUpdateBatch = TryEmplaceBatch<T>("LateUpdate");
			lateUpdateBatch->emplace_back(component, (ComponentCallBack::CallBack)&T::LateUpdate);
		}
		if constexpr (ComponentTrait<T>::hasFixedUpdate)
		{
			static std::vector<ComponentCallBack>* fixedUpdateBatch = TryEmplaceBatch<T>("FixedUpdate");
			fixedUpdateBatch->emplace_back(component, (ComponentCallBack::CallBack)&T::FixedUpdate);
		}
		if constexpr (ComponentTrait<T>::hasPreDraw)
		{
			static std::vector<ComponentCallBack>* preDrawBatch = TryEmplaceBatch<T>("PreDraw");
			preDrawBatch->emplace_back(component, (ComponentCallBack::CallBack)&T::PreDraw);
		}
		if constexpr (ComponentTrait<T>::hasDraw)
		{
			static std::vector<ComponentCallBack>* DrawBatch = TryEmplaceBatch<T>("Draw");
			DrawBatch->emplace_back(component, (ComponentCallBack::CallBack)&T::Draw);
		}
		if constexpr (ComponentTrait<T>::hasLateDraw)
		{
			static std::vector<ComponentCallBack>* LateDrawBatch = TryEmplaceBatch<T>("LateDraw");
			LateDrawBatch->emplace_back(component, (ComponentCallBack::CallBack)&T::LateDraw);
		}
	}

	template<typename T>
	inline std::vector<ComponentCallBack>* ComponentLoopSystem::TryEmplaceBatch(std::string_view loopState)
	{
		auto& batches = Instance().loopStates.try_emplace(loopState.data(), std::map<std::type_index, std::vector<ComponentCallBack>*>()).first->second;
		auto& batch = batches.try_emplace(typeid(T), new std::vector<ComponentCallBack>()).first->second;
		return batch;
	}

	template<typename T>
	inline void ComponentFactory::RegisterComponent(std::string_view classname)
	{
		auto ret = Instance().creators.try_emplace(classname.data(), []() {
			return Mrk::MemCtrlSystem::CreateNew<T>();
			});
		if (!ret.second)
		{
			//log
		}
	}

	template<typename T>
	inline std::shared_ptr<T> ComponentFactory::CreateNew()
	{
		static_assert(std::is_base_of_v<Component, T>, "T Is Not A Component !");
		return Mrk::MemCtrlSystem::CreateNew<T>();
	}
}

class TestComponent : public Mrk::Component
{
	MRK_COMPONENT(TestComponent)
	MRK_POOLABLE(TestComponent, 10)
public:
	inline void Update()
	{
		std::cout << "TestComponent Update" << "\n";
	}
};


