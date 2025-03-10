#include "Component.h"

#include <algorithm>

std::shared_ptr<Mrk::Component> Mrk::ComponentFactory::CreateNew(std::string_view classname)
{
	MRK_INSTANCE_REF;

	auto ret = instance.creators.find(classname.data());
	if (ret != instance.creators.end())
	{
		return ret->second();
	}
	
	return std::shared_ptr<Component>();
}

std::shared_ptr<Mrk::Component> Mrk::ComponentFactory::CreateNewFromJson(std::string_view classname, const Json::Value& json)
{
	MRK_INSTANCE_REF;

	auto ret = instance.fromJsonCreators.find(classname.data());
	if (ret != instance.fromJsonCreators.end())
	{
		return ret->second(json);
	}

	return std::shared_ptr<Component>();
}

const std::map<std::string, std::function<std::shared_ptr<Mrk::Component>()>>& Mrk::ComponentFactory::GetCreators()
{
	MRK_INSTANCE_REF;

	return instance.creators;
}

const std::vector<std::string>& Mrk::ComponentFactory::GetManifest()
{
	MRK_INSTANCE_REF;

	return instance.manifest;
}

std::weak_ptr<Mrk::GameObject> Mrk::Component::GetHolder()
{
	return holder;
}

bool Mrk::Component::IsRemovable()
{
	return true;
}

void Mrk::ComponentHut::Invoke(std::string_view loopBatchName)
{
	MRK_INSTANCE_REF;

	for (auto& start : instance.unstarts)
	{
		if (!start.Expired())
		{
			start.InvokeNotCheck();
		}
	}
	instance.unstarts.clear();

	size_t validCount = 0;
	size_t invalidCount = 0;
	auto loopBatch = instance.loopBatches.find(loopBatchName.data());
	if (loopBatch != instance.loopBatches.end())
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

void Mrk::ComponentHut::Cleanup()
{
	MRK_INSTANCE_REF;

	for (auto& loopBatch : instance.loopBatches)
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

void Mrk::ComponentHut::Cleanup(const LoopBatch& loopBacth)
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
	if (component)
	{
		componentPtr = component.get();
	}
}

const Mrk::Component* Mrk::ComponentCallBack::GetComponentPtr() const
{
	return componentPtr;
}

void Mrk::ComponentCallBack::Invoke() const
{
	if (!component.expired())
	{
		(component.lock().get()->*callback)();
	}
}

void Mrk::ComponentCallBack::InvokeNotCheck() const
{
	(component.lock().get()->*callback)();
}

bool Mrk::ComponentCallBack::Expired() const
{
	return component.expired();
}
