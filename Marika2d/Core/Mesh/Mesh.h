#pragma once

#include "Common/Singleton/Singleton.h"
#include "Common/Memory/MemCtrlSystem.h"

#include "Core/Math/Math.h"

#include "Third/glad/include/glad.h"
#include "Third/glm/glm.hpp"

#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <map>

#ifndef MRK_MESH_FILE_EXTENSION
#define MRK_MESH_FILE_EXTENSION ".mmsh"
#endif // !MRK_MESH_FILE_EXTENSION

namespace Mrk
{
	struct Vertex
	{
		Vertex();
		Vertex(const Vector3& position, const Vector3& normal, const Vector2& texcoord);
		Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty);
		Vector3 position;
		Vector3 normal;
		Vector2 texcoord;
	};

	struct SubMesh
	{
		GLuint offset;		//index offset
		GLuint count;		//index count
	};

	class Mesh
	{
		friend class MeshHut;
	public:
		Mesh();
		~Mesh();
		void Bind();

		const Vector3& GetBoundingMin();
		const Vector3& GetBoundingMax();
		const std::vector<SubMesh>& GetSubMeshes();

	private:
		std::vector<SubMesh> subMeshes;
		Vector3 max;
		Vector3 min;
		GLuint vao = 0;
		GLuint vbo = 0;
		GLuint ebo = 0;
	};

	class MeshHut : public Singleton<MeshHut>
	{
		MRK_SINGLETON(MeshHut)
	public:
		static std::shared_ptr<Mesh> GetMesh(std::string_view meshPath);

	private:
		MeshHut() = default;
		static std::shared_ptr<Mesh> LoadMesh(std::string_view path);
		static void LoadMeshGL(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Mesh> mesh);

	private:
		std::map<std::string, std::shared_ptr<Mesh>> meshs;
	};
}