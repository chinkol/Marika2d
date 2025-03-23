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
		friend class MeshRenderer;
	public:
		MaterialSlot();
		const std::string& GetName() const;
		const std::string& GetSpPath() const;
		void SetSpPath(const std::string& spPath);
		const std::string& GetMatPath() const;
		void SetMatPath(const std::string& matPath);
		std::shared_ptr<Material> GetMaterial() const;
		void Load();
	private:
		std::string name;
		std::string spPath;
		std::string matPath;
		std::shared_ptr<Material> material;
	};

	class MeshRenderer : public Component
	{
		MRK_COMPONENT(MeshRenderer) //MRK_POOLABLE(MeshRenderer, 100)
	public:
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