#pragma once

#include "Core/Component/Component.h"
#include "Core/OpenGL/OpenGL.h"
#include "Core/Scene/Scene.h"
#include "Core/Mesh/Mesh.h"

namespace Mrk
{
	class Material;

	class MaterialSlot
	{
	public:
		MaterialSlot();
		bool GetIsShared() const;
		void SetIsShared(bool val);
		const std::string& GetSpName() const;
		void SetSpName(const std::string& spName);
		const std::string& GetMatName() const;
		void SetMatName(const std::string& matName);
		std::shared_ptr<Material> GetMaterial() const;
		void Load();
	private:
		bool isShared;
		std::string spName;
		std::string matName;
		std::shared_ptr<Material> material;
	};

	class MeshRenderer : public Component
	{
		MRK_COMPONENT(MeshRenderer) MRK_POOLABLE(MeshRenderer, 100)
	public:
		void Start();
		void PreDraw();

		const std::string& GetMeshPath();
		void SetMeshPath(const std::string& meshPath);
		const std::vector<MaterialSlot>& GetMatSlots();
		void SetMatSlots(const std::vector<MaterialSlot>& matSlots);
	private:
		std::string meshPath;

	private:
		bool isDirty = true;
		std::shared_ptr<Mesh> mesh;
		std::vector<MaterialSlot> matSlots;
	};
}