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

	struct RenderItem
	{
		ID id;
		GLuint count;
		GLuint offset;
		Matrix4 world;
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> mat;
		std::shared_ptr<ShaderProgram> sp;
	};

	using RenderItemGroup = std::vector<RenderItem>;	// TODO : 缓存渲染项，不用每帧上传
	using RenderMeshGroups = std::map<std::shared_ptr<Mesh>, RenderItemGroup>;
	using RenderMatGroups = std::map<std::shared_ptr<Material>, RenderMeshGroups>;
	using RenderSpGroups = std::map<std::shared_ptr<ShaderProgram>, RenderMatGroups>;

	class RenderSys : public Singleton<RenderSys>
	{
		MRK_SINGLETON(RenderSys)
	public:
		static void Draw();
		static void Commit(const RenderItem& renderItem);
	private:
		RenderSpGroups spGroups;
	};
}