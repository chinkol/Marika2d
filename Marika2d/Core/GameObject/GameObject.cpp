#include "GameObject.h"

#include "Core/Component/Component.h"

void Mrk::GameObject::DeSerialize(const Json::Value& jobj)
{
	MRK_DESERIALIZE_FIELD(name, name);
	MRK_DESERIALIZE_FIELD(children, children);
}

void Mrk::GameObject::Serialize(Json::Value& jobj, JsonAlloc& jalloc) const
{
	MRK_SERIALIZE_CLASS(GameObject);

	MRK_SERIALIZE_FIELD(name);
	MRK_SERIALIZE_FIELD(children);
}

std::shared_ptr<Mrk::GameObject> Mrk::GameObjectFactory::CreateNew(std::string_view classname)
{
	auto ret = Instance().creators.find(classname.data());
	assert(ret != Instance().creators.end());
	return ret->second();
}

const std::map<std::string, std::function<std::shared_ptr<Mrk::GameObject>()>>& Mrk::GameObjectFactory::GetCreators()
{
	return Instance().creators;
}

void Mrk::GameObjectOperate::AttachChild(const std::shared_ptr<GameObject>& child, const std::shared_ptr<GameObject>& parent)
{
	assert(child && parent);
	if (!child->parent.expired())
	{
		auto oriParent = child->parent.lock();
		if (oriParent == parent)
			return;
		oriParent->children.erase(std::remove(oriParent->children.begin(), oriParent->children.end(), child), oriParent->children.end());
	}
	child->parent = parent;
	parent->children.push_back(child);
}

void Mrk::GameObjectOperate::DetachChild(const std::shared_ptr<GameObject>& child, const std::shared_ptr<GameObject>& parent)
{
	assert(child && parent);
	parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), child), parent->children.end());
	child->parent = std::weak_ptr<GameObject>();
}

void Mrk::GameObjectOperate::AttachComponent(const std::shared_ptr<Component>& component, const std::shared_ptr<GameObject>& holder)
{
	assert(component && holder);
	if (holder->components.try_emplace(component->GetClassName().data(), component).second)
	{
		component->holder = holder;
	}
	else
	{
		//log : repeat
	}
}

void Mrk::GameObjectOperate::AttachComponent(std::string_view comName, const std::shared_ptr<GameObject>& holder)
{
	AttachComponent(ComponentFactory::CreateNew(comName), holder);
}

void Mrk::GameObjectOperate::DetachComponent(const std::shared_ptr<Component>& component, const std::shared_ptr<GameObject>& holder)
{
	assert(component && holder);
	auto ret = holder->components.find(component->GetClassName().data());
	if (ret != holder->components.end())
	{
		holder->components.erase(ret);
		component->holder = std::weak_ptr<GameObject>();
	}
	else
	{
		//log : not found
	}
}

void Mrk::GameObjectOperate::DetachComponent(std::string_view comName, const std::shared_ptr<GameObject>& holder)
{
	assert(holder);
	auto ret = holder->components.find(comName.data());
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

std::shared_ptr<Mrk::Component> Mrk::GameObjectOperate::GetComponent(std::string_view comName, const std::shared_ptr<GameObject>& holder)
{
	assert(holder);
	auto ret = holder->components.find(comName.data());
	if (ret != holder->components.end())
	{
		return ret->second;
	}
	else
	{
		//log : not found
		return std::shared_ptr<Component>();
	}
}
