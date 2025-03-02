#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/OpenGL/OpenGL.h"
#include "Core/Render/Material/Material.h"
#include "Core/ID/IDCreater.h"
#include "Core/Math/Math.h"

#include <array>

namespace Mrk
{
	class Mesh;
	class Texture;

	enum class RenderLayer
	{
		BackGround = 0,
		Geometry = 1,
		Transparent = 2,
		ForeGround = 3
	};

	struct SubRenderParam
	{
		Material material;
		GLuint texture;
		GLuint shaderProgram;
	};

	struct ObjectRenderParam
	{
		ID id;
		Matrix4 world;
		std::shared_ptr<Mesh> mesh;
		std::vector<SubRenderParam> subRenderParams;
	};

	class RenderSys : public Singleton<RenderSys>
	{
		MRK_SINGLETON(RenderSys)
	public:
		void Draw();
		void Commit(RenderLayer renderLayer, const ObjectRenderParam& renderParam);
	private:
		std::array<std::vector<ObjectRenderParam>, 4> renderables;
	};
}


