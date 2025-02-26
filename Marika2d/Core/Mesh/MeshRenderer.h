#pragma once

#include "Core/Component/Component.h"
#include "Core/OpenGL/OpenGL.h"
#include "Core/Scene/Scene.h"

namespace Mrk
{
	class MeshRenderer : public Component
	{
		MRK_COMPONENT(MeshRenderer)
			MRK_POOLABLE(MeshRenderer, 100)
	public:
		const std::string& GetFsPath();
		const std::string& GetVsPath();
		void SetFsPath(const std::string& fs);
		void SetVsPath(const std::string& vs);
		void Start();
	private:
		void InitSp();
	private:
		std::string vs;
		std::string fs;
		GLuint sp;
	};
}
