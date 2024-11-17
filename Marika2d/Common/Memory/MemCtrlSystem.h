#pragma once

#include "Common/Singleton/Singleton.h"

#include <typeindex>
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <stack>
#include <map>

#ifndef MRK_POOLABLE
#define MRK_POOLABLE(x, y)	public:																\
static constexpr uint32_t _mrk_macro_poolable_alloc_count_ = y;									\
void* operator new(size_t size){																\
	return (void*)Mrk::MemCtrlSystem::Alloc<x>();}												\
void operator delete(void* ptr){																\
	Mrk::MemCtrlSystem::DeAlloc<x>(ptr);}
#endif // !MRK_POOLABLE

namespace Mrk
{
	namespace Internal
	{
		template <typename T, typename = void>
		struct HasPoolableMacro : std::false_type {};
		template <typename T>
		struct HasPoolableMacro<T, std::void_t<decltype(T::_mrk_macro_poolable_alloc_count_)>> : std::true_type {};

		class ObjectPoolBase
		{
		public:
			virtual void* Alloc() = 0;
			virtual void DeAlloc(void* ptr) = 0;
		};

		template<typename T, uint32_t Count>
		class ObjectPool : public ObjectPoolBase
		{
			using Buket = std::array<T, Count>;
		public:
			virtual void* Alloc() override
			{
				if (frees.empty())
				{
					CreateBuket();
				}
				auto free = frees.top();
				frees.pop();
				return free;
			}

			virtual void DeAlloc(void* ptr) override
			{
				if (ptr == nullptr)
					return;
				for (auto buket : bukets)
				{
					size_t distance = (char*)ptr - (char*)(buket->data());
					if (distance >= 0 && distance < sizeof(Buket) && distance % sizeof(T) == 0)
					{
						frees.push((T*)ptr);
						return;
					}
				}
				throw;
			}

		public:
			void CreateBuket()
			{
				auto BuketCapcity = Count;
				auto buketCount = bukets.size();

				auto newBuket = new Buket();
				bukets.emplace_back(newBuket);

				for (size_t i = 0; i < BuketCapcity; i++)
				{
					frees.push(newBuket->data() + i);
				}
			}

		private:
			std::stack<T*> frees;
			std::vector<Buket*>	bukets;
		};
	}
	
	class MemCtrlSystem : public Singleton<MemCtrlSystem>
	{
		MRK_SINGLETON(MemCtrlSystem)
	public:
		template<typename T>
		static void* Alloc()
		{
			static Internal::ObjectPool<T, T::_mrk_macro_poolable_alloc_count_>* objectPool = TryEmplaceObjPool<T>();
			return objectPool->Alloc();
		}

		template<typename T>
		static void DeAlloc(void* ptr)
		{
			static Internal::ObjectPool<T, T::_mrk_macro_poolable_alloc_count_>* objectPool = TryEmplaceObjPool<T>();
			objectPool->DeAlloc(ptr);
		}

		template<typename T, typename ...Args>
		static std::shared_ptr<T> CreateNew(Args&&... args)
		{
			if constexpr (Internal::HasPoolableMacro<T>::value)
			{
				return std::shared_ptr<T>(new T(std::forward<Args&&>(args)...));
			}
			else
			{
				return std::make_shared<T>(std::forward<Args&&>(args)...);
			}
		}

	private:
		template<typename T> static Internal::ObjectPool<T, T::_mrk_macro_poolable_alloc_count_>* TryEmplaceObjPool()
		{
			auto ret = Instance().objectPools.try_emplace(typeid(T), nullptr);
			if (ret.second)
			{
				ret.first->second = new Internal::ObjectPool<T, T::_mrk_macro_poolable_alloc_count_>();
			}
			return dynamic_cast<Internal::ObjectPool<T, T::_mrk_macro_poolable_alloc_count_>*>(ret.first->second);
		}
		std::map<std::type_index, Internal::ObjectPoolBase*> objectPools;
	};
}

class MemCtrlTest
{
	MRK_POOLABLE(MemCtrlTest, 10)
public:
	int field1 = 1;
	float field2 = 2.0;
	double field3 = 3.0;
};

inline void test()
{
	for (size_t i = 0; i < 1000; i++)
	{
		auto m = new MemCtrlTest();
		auto s = std::shared_ptr<MemCtrlTest>(m);
		auto s1 = s;
	}
}


