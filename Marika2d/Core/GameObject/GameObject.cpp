#include "GameObject.h"

#include "Core/Component/Component.h"

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

const std::vector<std::string>& Mrk::GameObjectFactory::GetManifest()
{
	return Instance().manifest;
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

	if (component->holder.lock() != holder)
	{
		holder->components.push_back(component);
		component->holder = holder;
	}
}

void Mrk::GameObjectOperate::AttachComponent(std::string_view comName, const std::shared_ptr<GameObject>& holder)
{
	AttachComponent(ComponentFactory::CreateNew(comName), holder);
}

void Mrk::GameObjectOperate::DetachComponent(const std::shared_ptr<Component>& component, const std::shared_ptr<GameObject>& holder)
{
	assert(component && holder);

	if (component->holder.lock() == holder)
	{
		holder->children.erase(std::find(holder->children.begin(), holder->children.end(), holder));
	}
}

void Mrk::GameObjectOperate::DetachComponent(std::string_view comName, const std::shared_ptr<GameObject>& holder)
{
	assert(holder);

	auto& children = holder->children;
	children.erase(std::remove_if(children.begin(), children.end(), [comName](const std::shared_ptr<GameObject>& child) {
		return child->GetClassName() == comName;
		}), children.end());
}

std::shared_ptr<Mrk::Component> Mrk::GameObjectOperate::GetComponent(std::string_view comName, const std::shared_ptr<GameObject>& holder)
{
	assert(holder);

	auto& components = holder->components;
	auto ret = std::find_if(components.begin(), components.end(), [comName](const std::shared_ptr<Component>& component) {
		return component->GetClassName() == comName;
		});

	if (ret != components.end())
	{
		return *ret;
	}

	return std::shared_ptr<Mrk::Component>();
}

const std::string& Mrk::GameObject::GetName()
{
	return name;
}

void Mrk::GameObject::SetName(const std::string& name)
{
	this->name = name;
}

void Mrk::GameObject::DeserializeGameObject(const Json::Value& json)
{
	if (json.IsObject())
	{
		auto jcomponents = json.FindMember("components");
		if (jcomponents != json.MemberEnd() && jcomponents->value.IsArray())
		{
			DeserializeComponents(jcomponents->value);
		}

		auto jchildren = json.FindMember("children");
		if (jchildren != json.MemberEnd() && jchildren->value.IsArray())
		{
			DeserializeChildren(jchildren->value);
		}
	}
}

void Mrk::GameObject::SerializeGameObject(Json::Value& json, Mrk::JsonAllocator& alloctor)
{
	json.AddMember(Json::Value(MRK_REFLECT_CLASS_JSON_PROP_NAME, alloctor), Json::Value(GetClassName().data(), alloctor), alloctor);
	json.AddMember(Json::Value("children", alloctor), SerializeChildren(alloctor), alloctor);
	json.AddMember(Json::Value("components", alloctor), SerializeComponents(alloctor), alloctor);
}

void Mrk::GameObject::DeserializeComponents(const Json::Value& jcomponents)
{
	for (auto& jcomponent : jcomponents.GetArray())
	{
		if (jcomponent.IsObject())
		{
			auto jname = jcomponent.FindMember(MRK_REFLECT_CLASS_JSON_PROP_NAME);
			if (jname != jcomponent.MemberEnd() && jname->value.IsString())
			{
				auto name = jname->value.GetString();
				if (auto component = ComponentFactory::CreateNew(name))
				{
					component->FromJson(jcomponent);
					auto shared = shared_from_this();
					GameObjectOperate::AttachComponent(component, shared);
				}
			}
		}
	}
}

void Mrk::GameObject::DeserializeChildren(const Json::Value& jchildren)
{
	for (auto& jchild : jchildren.GetArray())
	{
		if (jchild.IsObject())
		{
			auto jname = jchild.FindMember(MRK_REFLECT_CLASS_JSON_PROP_NAME);
			if (jname != jchild.MemberEnd() && jname->value.IsString())
			{
				auto name = jname->value.GetString();
				if (auto child = GameObjectFactory::CreateNew(name))
				{
					child->FromJson(jchild);
					GameObjectOperate::AttachChild(child, shared_from_this());
				}
			}
		}
	}
}

Json::Value Mrk::GameObject::SerializeComponents(Mrk::JsonAllocator& alloctor)
{
	Json::Value jarr = Json::Value(Json::ArrayType);
	for (auto& component : components)
	{
		jarr.PushBack(component->ToJson(alloctor), alloctor);
	}
	return jarr;
}

Json::Value Mrk::GameObject::SerializeChildren(Mrk::JsonAllocator& alloctor)
{
	Json::Value jarr(Json::ArrayType);
	for (auto& child : children)
	{
		jarr.PushBack(child->ToJson(alloctor), alloctor);
	}
	return jarr;
}