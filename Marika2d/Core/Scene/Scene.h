#pragma once

#include "Common/Singleton/Singleton.h"

#include "Third/glm/glm.hpp"

#include <array>
#include <vector>
#include <map>
#include <memory>
#include <string>

namespace Mrk
{
	//°Ë²æ

	struct OctElem
	{
		glm::vec3 min;
		glm::vec3 max;
	};

	struct OctNodeBordor
	{
		glm::vec3 innerMin;
		glm::vec3 innerMax;
		glm::vec3 outerMin;
		glm::vec3 outerMax;

		enum IntersctionType
		{
			Null,
			Cross,
			Include
		};

		OctNodeBordor(const glm::vec3& min, const glm::vec3& max);
		IntersctionType IntersctionDetect(const OctElem& elem);
		bool IsContainPoint(const glm::vec3& point);
		void Growth(const OctElem& elem);
	};

	struct OctNode;

	struct OctNodeBase
	{
		OctNode* parent;
		OctNodeBordor border;
		std::vector<OctElem> borderElems;

		OctNodeBase(const OctNodeBordor& border, OctNode* parent);
		virtual bool IsLeaf() = 0;
		virtual void Insert(const OctElem& elem) = 0;
	};

	struct OctLeaf;

	struct OctNode : public OctNodeBase
	{
		std::array<OctNodeBase*, 8> children = { nullptr };

		OctNode(const OctNodeBordor& border, OctNode* parent);
		virtual bool IsLeaf();
		virtual void Insert(const OctElem& elem);
		void SubDivide(size_t index, OctLeaf* oldLeaf);
	};

	struct OctLeaf : public OctNodeBase
	{
		std::vector<OctElem> elems;
		size_t index;

		OctLeaf(const OctNodeBordor& border, size_t index, OctNode* parent);
		virtual bool IsLeaf();
		virtual void Insert(const OctElem& elem);
	};

	class OctTree
	{
	public:
		OctTree(float width, float height, float depth);
		void Insert(const OctElem& elem);
	private:
		OctNode root;
	};

	class GameObject;

	class Scene
	{
		friend class SceneHut;
	public:
		std::shared_ptr<GameObject> GetRoot();
		void FromFile(std::string_view filename);
		void ToFile(std::string_view filename);
	private:
		Scene(std::string_view sceneName, float size);
	private:
		std::string name;
		std::shared_ptr<GameObject> root;
		OctTree octTree;
	};

	class SceneHut : Singleton<SceneHut>
	{
		MRK_SINGLETON(SceneHut)
	public:
		static std::shared_ptr<Scene> GetCurrScene();
		static void ChangeScene(std::string name);
		static void CreateNew(std::string_view sceneName, float size);
		static void FromFile(std::string_view fileName);
		static void ToFile(std::string_view fileDir);
	private:
		SceneHut() = default;
		std::shared_ptr<Scene> currScene;
		std::map<std::string, std::shared_ptr<Scene>> scenes;
	};
}