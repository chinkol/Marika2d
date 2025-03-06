#pragma once

#include "Core/Component/Component.h"
#include "Core/OpenGL/OpenGL.h"
#include "Core/Scene/Scene.h"
#include "Core/Mesh/Mesh.h"

namespace Mrk
{
	class MeshRenderer : public Component
	{
		MRK_COMPONENT(MeshRenderer) MRK_POOLABLE(MeshRenderer, 100)
	public:
		const std::string& GetMeshPath();
		const std::string& GetFsPath();
		const std::string& GetVsPath();
		void SetMeshPath(const std::string& meshPath);
		void SetFsPath(const std::string& fs);
		void SetVsPath(const std::string& vs);
		void Start();
		void PreDraw();
	private:
		void InitSp();
	private:
		std::string meshPath;
		std::string vsPath;
		std::string fsPath;

	private:
		std::shared_ptr<Mesh> mesh;
		GLuint sp;
	};
}