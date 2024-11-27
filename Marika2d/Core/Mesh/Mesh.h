#pragma once

#include "Common/Singleton/Singleton.h"
#include "Common/Memory/MemCtrlSystem.h"

#include "Third/glad/include/glad.h"
#include "Third/glm/glm.hpp"

#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace Mrk
{
	struct Vertex
	{
		Vertex();
		Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texcoord);
		Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty);
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};

	struct SubMesh
	{
		GLuint matIndex;	//material index
		GLuint offset;	//index offset
		GLuint count;		//index count
	};

	class Mesh
	{
		friend class MeshHouse;
	public:
		~Mesh();
		void Bind();

		const glm::vec3& GetBoundingMin();
		const glm::vec3& GetBoundingMax();
		const std::vector<SubMesh>& GetSubMeshes();

	private:
		Mesh();

	private:
		std::vector<SubMesh> subMeshes;
		glm::vec3 boundingMax;
		glm::vec3 boundingMin;
		GLuint vao = 0;
		GLuint vbo = 0;
		GLuint ebo = 0;
	};

	class MeshHouse : public Singleton<MeshHouse>
	{
		MRK_SINGLETON(MeshHouse)
	public:
		static std::shared_ptr<Mesh> GetMesh(std::string_view meshPath);

	private:
		MeshHouse() = default;
		static std::shared_ptr<Mesh> LoadMesh(std::string_view path);

	private:
		std::map<std::string, std::shared_ptr<Mesh>> meshs;
	};
}


