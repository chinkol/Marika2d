#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/OpenGL/OpenGL.h"
#include "Core/Render/Material/Material.h"
#include "Core/IDGenerater/IDGenerater.h"
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

	struct SubRenderItem
	{
		Material material;
		GLuint texture;
		GLuint shaderProgram;
	};

	struct RenderItem
	{
		ID id;
		std::shared_ptr<Mesh> mesh;
		Matrix4 world;
		GLuint sp;
	};

	class RenderSys : public Singleton<RenderSys>
	{
		MRK_SINGLETON(RenderSys)
	public:
		static void Draw();
		static void Commit(RenderLayer renderLayer, const RenderItem& renderParam);
	private:
		std::array<std::vector<RenderItem>, 4> renderLayers;
	};
}
