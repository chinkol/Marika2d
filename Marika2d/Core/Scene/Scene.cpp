#include "Scene.h"

#include "Core/GameObject/GameObject.h"

#include <filesystem>
#include <fstream>

#ifndef MRK_SCENE_EXTENSION
#define MRK_SCENE_EXTENSION ".msce"
#endif // !MRK_SCENE_EXTENSION


std::shared_ptr<Mrk::GameObject> Mrk::Scene::GetRoot()
{
	return root;
}

void Mrk::Scene::FromFile(std::string_view filename)
{
	std::filesystem::path fromPath = filename.data();
	if (std::filesystem::exists(fromPath) && fromPath.extension() == MRK_SCENE_EXTENSION)
	{
		std::ifstream ifstream(fromPath);
		if (ifstream.is_open())
		{
			std::string content;
			char ch;
			while (ifstream.get(ch))
			{
				content += ch;
			}
			ifstream.close();

			Json::Document jdoc;
			jdoc.Parse(content); if (!jdoc.HasParseError())
			{
				root->FromJson(jdoc);
			}
		}
	}
}

void Mrk::Scene::ToFile(std::string_view filename)
{
	std::filesystem::path toPath = filename.data();
	if (toPath.extension() == MRK_SCENE_EXTENSION)
	{
		if (!std::filesystem::exists(toPath.parent_path()))
		{
			std::filesystem::create_directories(toPath.parent_path());
		}

		std::ofstream ofstream(toPath);
		if (ofstream.is_open())
		{
			Json::StringBuffer buf;
			Json::PrettyWriter writer(buf);

			Json::Document jdoc;
			auto json = root->ToJson(jdoc.GetAllocator());
			json.Accept(writer);

			ofstream << buf.GetString() << std::endl;
			ofstream.close();
		}
	}
}

Mrk::Scene::Scene(std::string_view sceneName, float size) :
	name(sceneName),
	octTree(size, size, size)
{

}

Mrk::OctNodeBordor::OctNodeBordor(const glm::vec3& min, const glm::vec3& max) :
	innerMin(min),
	outerMin(min),
	innerMax(max),
	outerMax(max)
{

}

Mrk::OctNodeBordor::IntersctionType Mrk::OctNodeBordor::IntersctionDetect(const OctElem& elem)
{
	if (elem.max.x < outerMin.x || elem.min.x > outerMax.x ||
		elem.max.y < outerMin.y || elem.min.y > outerMax.y ||
		elem.max.z < outerMin.z || elem.min.z > outerMax.z)
	{
		return IntersctionType::Null;
	}

	if (elem.min.x >= innerMin.x && elem.max.x <= innerMax.x &&
		elem.min.y >= innerMin.y && elem.max.y <= innerMax.y &&
		elem.min.z >= innerMin.z && elem.max.z <= innerMax.z)
	{
		return IntersctionType::Include;
	}

	return IntersctionType::Cross;
}

bool Mrk::OctNodeBordor::IsContainPoint(const glm::vec3& point)
{
	return (point.x >= innerMin.x && point.x <= innerMax.x) &&
		(point.y >= innerMin.y && point.y <= innerMax.y) &&
		(point.z >= innerMin.z && point.z <= innerMax.z);
}

void Mrk::OctNodeBordor::Growth(const OctElem& elem)
{
	outerMin = glm::vec3(std::min(outerMin.x, elem.min.x), std::min(outerMin.y, elem.min.y), std::min(outerMin.z, elem.min.z));
	outerMax = glm::vec3(std::max(outerMax.x, elem.max.x), std::max(outerMax.y, elem.max.y), std::max(outerMax.z, elem.max.z));
}

Mrk::OctNodeBase::OctNodeBase(const OctNodeBordor& border, OctNode* parent) :
	border(border),
	parent(parent)
{

}

Mrk::OctNode::OctNode(const OctNodeBordor& border, OctNode* parent) :
	OctNodeBase(border, parent)
{
	auto half = (border.innerMax - border.innerMin) / 2.0f;
	auto halfWidth = half.x;
	auto halfHeight = half.y;
	auto halfDepth = half.z;
	for (size_t i = 0; i < 2; i++)
	{
		auto leafMin = border.innerMin + glm::vec3(halfWidth * i, 0, 0);
		for (size_t j = 0; j < 2; j++)
		{
			leafMin += glm::vec3(0, halfHeight * j, 0);
			for (size_t k = 0; k < 2; k++)
			{
				leafMin += glm::vec3(0, 0, halfDepth * k);
				auto leafMax = leafMin + half;
				auto leafBorder = OctNodeBordor(leafMin, leafMax);
				auto leafIndex = (i << 2) | (j << 1) | k;
				auto newLeaf = new OctLeaf(leafBorder, (size_t)leafIndex, this);
				children[leafIndex] = (newLeaf);
			}
		}
	}
}

bool Mrk::OctNode::IsLeaf() { return false; }

void Mrk::OctNode::Insert(const OctElem& elem)
{
	for (auto child : children)
	{
		auto ret = child->border.IntersctionDetect(elem);
		if (ret == OctNodeBordor::Include)
		{
			child->Insert(elem);
			return;
		}
		else if (ret == OctNodeBordor::Cross)
		{
			glm::vec3 elemCenter = (elem.max + elem.min) / 2.0f;
			if (child->border.IsContainPoint(elemCenter))
			{
				child->border.Growth(elem);
				child->borderElems.push_back(elem);
				return;
			}
		}
	}
	throw; //out of root border
}

void Mrk::OctNode::SubDivide(size_t index, OctLeaf* oldLeaf)
{
	auto& leafElems = oldLeaf->elems;
	auto newNode = new OctNode(oldLeaf->border, this);
	for (auto& elem : leafElems)
	{
		newNode->Insert(elem);
	}
	children[index] = newNode;
	delete oldLeaf;
}

Mrk::OctLeaf::OctLeaf(const OctNodeBordor& border, size_t index, OctNode* parent) :
	OctNodeBase(border, parent),
	index(index)
{

}

bool Mrk::OctLeaf::IsLeaf()
{
	return true;
}

void Mrk::OctLeaf::Insert(const OctElem& elem)
{
	if (elems.size() < 4)
	{
		elems.push_back(elem);
	}
	else
	{
		parent->SubDivide(index, this);
		return;
	}
}

Mrk::OctTree::OctTree(float width, float height, float depth) :
	root(OctNodeBordor({ -width / 2, -height / 2, -depth / 2 }, { width / 2, height / 2, depth / 2 }), nullptr)
{
}

void Mrk::OctTree::Insert(const OctElem& elem)
{
	root.Insert(elem);
}

std::shared_ptr<Mrk::Scene> Mrk::SceneHut::GetCurrScene()
{
	return Instance().currScene;
}

void Mrk::SceneHut::ChangeScene(std::string name)
{
	auto ret = Instance().scenes.find(name.data());
	if (ret != Instance().scenes.end())
	{
		Instance().currScene = ret->second;
	}
	else
	{
		throw; //log : not found
	}
}

void Mrk::SceneHut::CreateNew(std::string_view sceneName, float size)
{
	Instance().currScene = Instance().scenes.try_emplace(sceneName.data(), [sceneName, size]() {
		auto newScene = std::shared_ptr<Scene>(new Scene(sceneName, size));
		newScene->root = GameObjectFactory::CreateNew<GameObject>();
		newScene->root->SetName(sceneName.data());
		return newScene;
		}()).first->second;
}

void Mrk::SceneHut::FromFile(std::string_view fileName)
{
	auto scene = std::shared_ptr<Scene>(new Scene("Scene", 1000));
	scene->root = GameObjectFactory::CreateNew<GameObject>();
	scene->root->SetName("Scene");
	scene->FromFile(fileName);
	Instance().currScene = scene;
}

void Mrk::SceneHut::ToFile(std::string_view fileName)
{
	Instance().currScene->ToFile(fileName);
}