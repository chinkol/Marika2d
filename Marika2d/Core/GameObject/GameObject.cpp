#include "GameObject.h"

#include "Core/Component/Component.h"
#include "Core/IDGenerater/IDGenerater.h"

std::shared_ptr<Mrk::GameObject> Mrk::GameObjectFactory::CreateNew(std::string_view classname)
{
	auto ret = Instance().creators.find(classname.data());
	assert(ret != Instance().creators.end());
	auto obj = ret->second();
	GameObjectHut::AddObject(obj);
	obj->SetName(obj->GetClassTypeName().data());
	obj->AddComponent<Transform>();
	obj->Init();
	return obj;
}

const std::map<std::string, std::function<std::shared_ptr<Mrk::GameObject>()>>& Mrk::GameObjectFactory::GetCreators()
{
	return Instance().creators;
}

const std::vector<std::string>& Mrk::GameObjectFactory::GetManifest()
{
	return Instance().manifest;
}

Mrk::GameObject::GameObject() :
	id(Mrk::IDGenerater::Generate())
{
}

Mrk::ID Mrk::GameObject::GetID()
{
	return id;
}

void Mrk::GameObject::SetID_(ID id)
{
	this->id = id;
}

const std::string& Mrk::GameObject::GetName()
{
	return name;
}

void Mrk::GameObject::SetName(const std::string& name)
{
	this->name = name;
}

std::shared_ptr<Mrk::GameObject> Mrk::GameObject::GetParent()
{
	if (!parent.expired())
	{
		return parent.lock();
	}
	return std::shared_ptr<GameObject>();
}

const std::vector<std::shared_ptr<Mrk::GameObject>>& Mrk::GameObject::GetChildren()
{
	return children;
}

void Mrk::GameObject::AddComponent(std::string_view name)
{
	auto ret = components.find(name.data());
	if (ret == components.end())
	{
		components.emplace(name.data(), [name, this]() {
			auto com = ComponentFactory::CreateNew(name.data());
			com->holder = weak_from_this();
			return com;
			}());
	}
}

void Mrk::GameObject::AddComponent(std::shared_ptr<Component> component)
{
	if (component)
	{
		if (component->holder.expired())
		{
			if (components.try_emplace(component->GetClassTypeName().data(), component).second)
			{
				component->holder = weak_from_this();
			}
		}
		else if (component->holder.lock().get() != this)
		{
			if (components.try_emplace(component->GetClassTypeName().data(), component).second)
			{
				component->holder.lock()->RemoveComponent(component->GetClassTypeName());
				component->holder = weak_from_this();
			}
		}
	}
}

void Mrk::GameObject::RemoveComponent(std::string_view name)
{
	auto ret = components.find(name.data());
	if (ret != components.end() && ret->second->IsRemovable())
	{
		components.erase(ret);
	}
}

std::shared_ptr<Mrk::Component> Mrk::GameObject::GetComponent(std::string_view name)
{
	auto ret = components.find(name.data());
	if (ret != components.end())
	{
		return ret->second;
	}

	return std::shared_ptr<Component>();
}

void Mrk::GameObject::AddChild(std::shared_ptr<GameObject> child)
{
	if (child->parent.expired())
	{
		child->parent = weak_from_this();
		children.push_back(child);
	}
	else if (child->parent.owner_before(weak_from_this()) || weak_from_this().owner_before(child->parent))
	{
		child->parent.lock()->RemoveChild(child);
		child->parent = weak_from_this();
		children.push_back(child);
	}
}

void Mrk::GameObject::RemoveChild(std::shared_ptr<GameObject> child)
{
	children.erase(std::find(children.begin(), children.end(), child));
}

void Mrk::GameObject::Init()
{
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
	json.AddMember(Json::Value(MRK_REFLECT_CLASS_JSON_PROP_NAME, alloctor), Json::Value(this->GetClassTypeName().data(), alloctor), alloctor);
	json.AddMember(Json::Value("children", alloctor), SerializeChildren(alloctor), alloctor);
	json.AddMember(Json::Value("components", alloctor), SerializeComponents(alloctor), alloctor);
}

void Mrk::GameObject::DeserializeComponents(const Json::Value& jcomponents)
{
	assert(jcomponents.IsArray());

	for (auto& jcomponent : jcomponents.GetArray())
	{
		if (jcomponent.IsObject())
		{
			auto jname = jcomponent.FindMember(MRK_REFLECT_CLASS_JSON_PROP_NAME);
			if (jname != jcomponent.MemberEnd() && jname->value.IsString())
			{
				auto name = jname->value.GetString();
				auto ret = components.find(name);
				if (ret != components.end())
				{
					ret->second->FromJson(jcomponent);
				}
				else
				{
					AddComponent(ComponentFactory::CreateNewFromJson(name, jcomponent));
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
					AddChild(child);
				}
			}
		}
	}
}

Json::Value Mrk::GameObject::SerializeComponents(Mrk::JsonAllocator& alloctor)
{
	Json::Value jarr = Json::Value(Json::ArrayType);
	for (auto& [_, component] : components)
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

std::shared_ptr<Mrk::GameObject> Mrk::GameObjectHut::GetObejct(uint64_t id)
{
	auto ret = Instance().objects.find(id);
	if (ret != Instance().objects.end())
	{
		if (!ret->second.expired())
		{
			return ret->second.lock();
		}
	}return std::shared_ptr<GameObject>();
}

void Mrk::GameObjectHut::AddObject(std::shared_ptr<GameObject> gameObject)
{
	if (!Instance().objects.try_emplace(gameObject->GetID().total64, gameObject).second)
	{
		throw;
	}
}