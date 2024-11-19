#include "Component.h"

#include <algorithm>

std::shared_ptr<Mrk::Component> Mrk::ComponentFactory::CreateNew(std::string_view classname)
{
	auto ret = Instance().creators.find(classname.data());
	assert(ret != Instance().creators.end());	//Forget To Add 'MRK_COMPONENT(x)' To Component Header ?
	return ret->second();
}

const std::map<std::string, std::function<std::shared_ptr<Mrk::Component>()>>& Mrk::ComponentFactory::GetCreators()
{
	return Instance().creators;
}

std::weak_ptr<Mrk::GameObject> Mrk::Component::GetHolder()
{
	return holder;
}

void Mrk::ComponentHouse::Invoke(std::string_view loopBatchName)
{
	size_t validCount = 0;
	size_t invalidCount = 0;
	auto loopBatch = Instance().loopBatches.find(loopBatchName.data());
	if (loopBatch != Instance().loopBatches.end())
	{
		for (auto& typeBatch : loopBatch->second)
		{
			auto& [_, callbacks] = typeBatch;
			for (auto& callback : *callbacks)
			{
				if (!callback.Expired())
				{
					callback.InvokeNotCheck();
					++validCount;
				}
				else
				{
					++invalidCount;
				}
			}
		}
	}
	if ((float)invalidCount / (float)validCount > 0.5f)
	{
		Cleanup(loopBatch->second);	//失效个数超过1/3，启动清理
	}
}

void Mrk::ComponentHouse::Cleanup()
{
	for (auto& loopBatch : Instance().loopBatches)
	{
		auto& [_, typeBatch] = loopBatch;
		for (auto& [_, callbacks] : typeBatch)
		{
			//按内存顺序排序后清理
			std::sort(callbacks->begin(), callbacks->end(), [](const ComponentCallBack& c1, const ComponentCallBack& c2) {
				if (c1.Expired())
					return false;
				if (c2.Expired())
					return true;
				return c1.GetComponentPtr() < c2.GetComponentPtr();
				});
			callbacks->erase(std::remove_if(callbacks->begin(), callbacks->end(), [](ComponentCallBack& callback) { return callback.Expired(); }), callbacks->end());
		}
	}
}

void Mrk::ComponentHouse::Cleanup(const LoopBatch& loopBacth)
{
	for (auto& typeBatch : loopBacth)
	{
		auto& [_, callbacks] = typeBatch;
		//按内存顺序排序后清理
		std::sort(callbacks->begin(), callbacks->end(), [](const ComponentCallBack& c1, const ComponentCallBack& c2) {
			if (c1.Expired())
				return false;
			if (c2.Expired())
				return true;
			return c1.GetComponentPtr() < c2.GetComponentPtr();
			});
		callbacks->erase(std::remove_if(callbacks->begin(), callbacks->end(), [](ComponentCallBack& callback) { return callback.Expired(); }), callbacks->end());
	}
}

Mrk::ComponentCallBack::ComponentCallBack(const std::shared_ptr<Component>& component, CallBack callback) :
	component(component),
	callback(callback)
{
}

const Mrk::Component* Mrk::ComponentCallBack::GetComponentPtr() const
{
	return componentPtr;
}

void Mrk::ComponentCallBack::Invoke() const
{
	auto scom = component.lock();
	//if (scom->Enable())
	{
		(scom.get()->*callback)();
	}
}

void Mrk::ComponentCallBack::InvokeNotCheck() const
{
	(componentPtr->*callback)();
}

bool Mrk::ComponentCallBack::Expired() const
{
	return component.expired();
}
