#pragma once

#include "Core/Component/Component.h"
#include "Core/OpenGL/OpenGL.h"
#include "Core/Scene/Scene.h"

namespace Mrk
{
	class Mesh;
	class Material;
	class ShaderProgram;

	class MaterialSlot
	{
		friend class MeshRenderer;
	public:
		MaterialSlot();
		const std::string& GetName() const;
		const std::string& GetMatPath() const;
		void SetMatPath(const std::string& matPath);
		const std::string& GetSpPath() const;
		void SetSpPath(const std::string& spPath);
		std::shared_ptr<Material> GetMaterial() const;
		std::shared_ptr<ShaderProgram> GetShaderProgram() const;
	private:
		std::string name;
		std::string spPath;
		std::string matPath;
		std::shared_ptr<ShaderProgram> shaderProgram;
		std::shared_ptr<Material> material;
	};

	class MeshRenderer : public Component
	{
		MRK_COMPONENT(MeshRenderer) //MRK_POOLABLE(MeshRenderer, 100)
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
		bool isMeshDirty = true;
		std::shared_ptr<Mesh> mesh;
		std::vector<MaterialSlot> matSlots;
	};
}
