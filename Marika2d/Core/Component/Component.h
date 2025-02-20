#pragma once

#include "Core/Object/Object.h"
#include "Core/Reflect/Reflect.h"

#include "Common/Singleton/Singleton.h"
#include "Common/Memory/MemCtrlSystem.h"

#include <map>
#include <string>

#ifndef MRK_COMPONENT_CONTENT
#define MRK_COMPONENT_CONTENT(x)																										\
MRK_OBJECT(x)																															\
friend struct Mrk::ComponentTrait<x>;																									\
static inline bool _mrk_macro_##x##_component_register_ = [](){																			\
	Mrk::ComponentFactory::RegisterComponent<x>(#x); return true;																		\
	}();																																\
virtual inline Json::Value ToJson(Mrk::JsonAllocator& alloctor) override {																\
	auto json = Mrk::ReflectSys::ToJson(*this, alloctor);																				\
	json.AddMember(Json::Value(MRK_REFLECT_CLASS_JSON_PROP_NAME, alloctor), Json::Value(GetClassName().data(), alloctor), alloctor);	\
	return json;																														\
}																																		\
virtual inline void FromJson(const Json::Value& json) override {																		\
	Mrk::ReflectSys::FromJson(*this, json);																								\
}
#endif // !MRK_COMPONENT_CONTENT

#ifndef MRK_COMPONENT
#define MRK_COMPONENT(x)		\
MRK_COMPONENT_CONTENT(x)		\
RTTR_ENABLE(Mrk::Component)
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

	class ComponentCallBack
	{
	public:
		using CallBack = void(Component::*)();
		ComponentCallBack(const std::shared_ptr<Component>& component, CallBack callback);
		const Component* GetComponentPtr() const;	//maybe nullptr
		void Invoke() const;
		void InvokeNotCheck() const;
		bool Expired() const;
	private:
		Component* componentPtr = nullptr;
		std::weak_ptr<Component> component;
		CallBack callback;
	};

	template<typename T>
	struct ComponentTrait
	{
		template <typename U> static auto HasAwake(U* ptr) -> decltype(ptr->Awake(), std::true_type()) {};
		template <typename U> static std::false_type HasAwake(...) {};
		static ComponentCallBack::CallBack GetAwake() { return (ComponentCallBack::CallBack)&T::Awake; }

		template <typename U> static auto HasStart(U* ptr) -> decltype(ptr->Start(), std::true_type()) {};
		template <typename U> static std::false_type HasStart(...) {};
		static ComponentCallBack::CallBack GetStart() { return (ComponentCallBack::CallBack)&T::Start; }

		template <typename U> static auto HasPreUpdate(U* ptr) -> decltype(ptr->PreUpdate(), std::true_type()) {};
		template <typename U> static std::false_type HasPreUpdate(...) {};
		static ComponentCallBack::CallBack GetPreUpdate() { return (ComponentCallBack::CallBack)&T::PreUpdate; }

		template <typename U> static auto HasUpdate(U* ptr) -> decltype(ptr->Update(), std::true_type()) {};
		template <typename U> static std::false_type HasUpdate(...) {};
		static ComponentCallBack::CallBack GetUpdate() { return (ComponentCallBack::CallBack)&T::Update; }

		template <typename U> static auto HasLateUpdate(U* ptr) -> decltype(ptr->LateUpdate(), std::true_type()) {};
		template <typename U> static std::false_type HasLateUpdate(...) {};
		static ComponentCallBack::CallBack GetLateUpdate() { return (ComponentCallBack::CallBack)&T::LateUpdate; }

		template <typename U> static auto HasFixedUpdate(U* ptr) -> decltype(ptr->FixedUpdate(), std::true_type()) {};
		template <typename U> static std::false_type HasFixedUpdate(...) {};
		static ComponentCallBack::CallBack GetFixedUpdate() { return (ComponentCallBack::CallBack)&T::FixedUpdate; }

		template <typename U> static auto HasPreDraw(U* ptr) -> decltype(ptr->PreDraw(), std::true_type()) {};
		template <typename U> static std::false_type HasPreDraw(...) {};
		static ComponentCallBack::CallBack GetPreDraw() { return (ComponentCallBack::CallBack)&T::PreDraw; }

		template <typename U> static auto HasDraw(U* ptr) -> decltype(ptr->Draw(), std::true_type()) {};
		template <typename U> static std::false_type HasDraw(...) {};
		static ComponentCallBack::CallBack GetDraw() { return (ComponentCallBack::CallBack)&T::Draw; }

		template <typename U> static auto HasLateDraw(U* ptr) -> decltype(ptr->LateDraw(), std::true_type()) {};
		template <typename U> static std::false_type HasLateDraw(...) {};
		static ComponentCallBack::CallBack GetLateDraw() { return (ComponentCallBack::CallBack)&T::LateDraw; }

		template <typename U> static auto HasDispose(U* ptr) -> decltype(ptr->Dispose(), std::true_type()) {};
		template <typename U> static std::false_type HasDispose(...) {};
		static ComponentCallBack::CallBack GetDispose() { return (ComponentCallBack::CallBack)&T::Dispose; }

		static constexpr bool hasAwake = decltype(HasAwake<T>(nullptr))::value;
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

	class Component : public Object, public std::enable_shared_from_this<Component>
	{
		MRK_COMPONENT_CONTENT(Component) RTTR_ENABLE(Object)
			friend class GameObjectOperate;
	public:
		Component() = default;
		virtual ~Component() = default;
		std::weak_ptr<GameObject> GetHolder();
	protected:
		std::weak_ptr<GameObject> holder;
	};

	class ComponentHouse : public Singleton<ComponentHouse>
	{
		MRK_SINGLETON(ComponentHouse)
			using TypeBatch = std::vector<ComponentCallBack>;
		using LoopBatch = std::map<std::type_index, TypeBatch*>;
	public:
		static void Invoke(std::string_view loopState);
		static void Cleanup();
		template<typename T> static void AddComponent(const std::shared_ptr<T>& component);

	private:
		ComponentHouse() = default;
		static void Cleanup(const LoopBatch& batch);
		template<typename T> static TypeBatch* TryEmplaceBatch(std::string_view loopState);
		std::map<std::string, LoopBatch> loopBatches; // [loopstate, [comtype, {loopfuncs}]];
	};
}

namespace Mrk
{
	template<typename T>
	inline void ComponentHouse::AddComponent(const std::shared_ptr<T>& component)
	{
		assert(component);
		if constexpr (ComponentTrait<T>::hasPreUpdate)
		{
			static std::vector<ComponentCallBack>* preUpdateBatch = TryEmplaceBatch<T>("PreUpdate");
			preUpdateBatch->emplace_back(component, ComponentTrait<T>::GetPreUpdate());
		}
		if constexpr (ComponentTrait<T>::hasUpdate)
		{
			static std::vector<ComponentCallBack>* updateBatch = TryEmplaceBatch<T>("Update");
			updateBatch->emplace_back(component, ComponentTrait<T>::GetUpdate());
		}
		if constexpr (ComponentTrait<T>::hasLateUpdate)
		{
			static std::vector<ComponentCallBack>* lateUpdateBatch = TryEmplaceBatch<T>("LateUpdate");
			lateUpdateBatch->emplace_back(component, ComponentTrait<T>::GetLateUpdate());
		}
		if constexpr (ComponentTrait<T>::hasFixedUpdate)
		{
			static std::vector<ComponentCallBack>* fixedUpdateBatch = TryEmplaceBatch<T>("FixedUpdate");
			fixedUpdateBatch->emplace_back(component, ComponentTrait<T>::GetFixedUpdate());
		}
		if constexpr (ComponentTrait<T>::hasPreDraw)
		{
			static std::vector<ComponentCallBack>* preDrawBatch = TryEmplaceBatch<T>("PreDraw");
			preDrawBatch->emplace_back(component, ComponentTrait<T>::GetPreDraw());
		}
		if constexpr (ComponentTrait<T>::hasDraw)
		{
			static std::vector<ComponentCallBack>* DrawBatch = TryEmplaceBatch<T>("Draw");
			DrawBatch->emplace_back(component, ComponentTrait<T>::GetDraw());
		}
		if constexpr (ComponentTrait<T>::hasLateDraw)
		{
			static std::vector<ComponentCallBack>* LateDrawBatch = TryEmplaceBatch<T>("LateDraw");
			LateDrawBatch->emplace_back(component, ComponentTrait<T>::GetLateDraw());
		}
	}

	template<typename T>
	inline Mrk::ComponentHouse::TypeBatch* ComponentHouse::TryEmplaceBatch(std::string_view loopState)
	{
		auto& loopBacth = Instance().loopBatches.try_emplace(loopState.data(), std::map<std::type_index, std::vector<ComponentCallBack>*>()).first->second;
		auto& batch = loopBacth.try_emplace(typeid(T), new std::vector<ComponentCallBack>()).first->second;
		return batch;
	}

	template<typename T>
	inline void ComponentFactory::RegisterComponent(std::string_view classname)
	{
		auto ret = Instance().creators.try_emplace(classname.data(), []() {
			auto newCom = Mrk::MemCtrlSystem::CreateNew<T>();
			ComponentHouse::AddComponent(newCom);
			if constexpr (ComponentTrait<T>::hasAwake)
			{
				(newCom.get()->*ComponentTrait<T>::GetAwake())();
			}
			return newCom;
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
		auto newCom = Mrk::MemCtrlSystem::CreateNew<T>();
		ComponentHouse::AddComponent(newCom);
		if constexpr (ComponentTrait<T>::hasAwake)
		{
			(newCom.get()->*ComponentTrait<T>::GetAwake())();
		}
		return newCom;
	}
}
