#include "Mesh.h"

#include <filesystem>
#include <fstream>

Mrk::Vertex::Vertex() :
	position(0, 0, 0),
	normal(0, 0, 0),
	texcoord(0, 0)
{
}

Mrk::Vertex::Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texcoord) :
	position(position),
	normal(normal),
	texcoord(texcoord)
{
}

Mrk::Vertex::Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty) :
	position(px, py, pz),
	normal(nx, ny, nz),
	texcoord(tx, ty)
{
}

Mrk::Mesh::Mesh() : 
	boundingMin({0, 0, 0}),
	boundingMax({0, 0, 0})
{
}

Mrk::Mesh::~Mesh()
{
}

void Mrk::Mesh::Bind()
{
}

const glm::vec3& Mrk::Mesh::GetBoundingMin()
{
	return boundingMin;
}

const glm::vec3& Mrk::Mesh::GetBoundingMax()
{
	return boundingMax;
}

const std::vector<Mrk::SubMesh>& Mrk::Mesh::GetSubMeshes()
{
	return subMeshes;
}

std::shared_ptr<Mrk::Mesh> Mrk::MeshHouse::GetMesh(std::string_view meshPath)
{
	auto ret = Instance().meshs.try_emplace(meshPath.data(), nullptr);
	if (ret.second)
	{
		ret.first->second = LoadMesh(meshPath);
	}
	return ret.first->second;
}

std::shared_ptr<Mrk::Mesh> Mrk::MeshHouse::LoadMesh(std::string_view path)
{
	std::filesystem::path mem(path);
	if (std::filesystem::exists(mem) && mem.extension() == ".mem")
	{
		std::ifstream stream(mem, std::ios::binary);
		if (stream.is_open())
		{
			char head[80];
			stream.read(head, 80);

			uint32_t vertexCount = 0;
			stream.read((char*)&vertexCount, sizeof(uint32_t));

			std::vector<Vertex> vertices(vertexCount, Vertex());
			stream.read((char*)vertices.data(), sizeof(Vertex) * vertexCount);

			uint32_t indexCount = 0;
			stream.read((char*)&indexCount, sizeof(uint32_t));

			std::vector<uint32_t> indices(indexCount, 0);
			stream.read((char*)indices.data(), sizeof(uint32_t) * indexCount);

			return std::shared_ptr<Mesh>(new Mesh());
		}
	}

	//log : file error
	return std::shared_ptr<Mesh>(new Mesh());
}
