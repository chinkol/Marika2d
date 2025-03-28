#include "Mesh.h"

#include <filesystem>
#include <fstream>

Mrk::Vertex::Vertex() :
	position(0, 0, 0),
	normal(0, 0, 0),
	texcoord(0, 0)
{
}

Mrk::Vertex::Vertex(const Vector3& position, const Vector3& normal, const Vector2& texcoord) :
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
	min({ 0, 0, 0 }),
	max({ 0, 0, 0 })
{
}

Mrk::Mesh::~Mesh()
{
}

void Mrk::Mesh::Bind()
{
	glBindVertexArray(vao);
}

void Mrk::Mesh::UnBind()
{
	glBindVertexArray(0);
}

const Mrk::Vector3& Mrk::Mesh::GetBoundingMin()
{
	return min;
}

const Mrk::Vector3& Mrk::Mesh::GetBoundingMax()
{
	return max;
}

const std::vector<Mrk::SubMesh>& Mrk::Mesh::GetSubMeshes()
{
	return subMeshes;
}

const std::vector<std::string>& Mrk::Mesh::GetSubMeshNames()
{
	return subMeshNames;
}

std::shared_ptr<Mrk::Mesh> Mrk::MeshHut::GetMesh(std::string_view meshPath)
{
	MRK_INSTANCE_REF;

	auto ret = instance.meshs.find(meshPath.data());
	if (ret != instance.meshs.end())
	{
		return ret->second;
	}
	else if (auto mesh = LoadMesh(meshPath))
	{
		instance.meshs.emplace(meshPath, mesh);
		return mesh;
	}
	else
	{
		return std::shared_ptr<Mesh>();
	}
}

namespace
{
	void CalMeshAABBBound(const std::vector<Mrk::Vertex>& vertices, Mrk::Vector3& max, Mrk::Vector3 min)
	{
		constexpr auto minf = std::numeric_limits<float>::min();
		constexpr auto maxf = std::numeric_limits<float>::max();

		max = Mrk::Vector3(minf);
		min = Mrk::Vector3(maxf);

		for (auto& vertex : vertices)
		{
			const Mrk::Vector3& pos = vertex.position;

			max.x = std::max(max.x, pos.x);
			max.y = std::max(max.y, pos.y);
			max.z = std::max(max.z, pos.z);

			min.x = std::min(min.x, pos.x);
			min.y = std::min(min.y, pos.y);
			min.z = std::min(min.z, pos.z);
		}
	}
}

std::shared_ptr<Mrk::Mesh> Mrk::MeshHut::LoadMesh(std::string_view path)
{
	std::filesystem::path mesh(path);
	if (std::filesystem::exists(mesh) && mesh.extension() == MRK_MESH_FILE_EXTENSION)
	{
		std::ifstream stream(mesh, std::ios::binary);
		if (stream.is_open())
		{
			auto mesh = std::make_shared<Mesh>();

			char head[80];
			stream.read(head, 80);

			uint32_t indexOffset = 0;

			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			while (!stream.eof())
			{
				// 读取名称长度
				uint32_t nameLength = 0;
				stream.read(reinterpret_cast<char*>(&nameLength), sizeof(uint32_t));

				// 读取名称数据
				std::string subMeshName(nameLength, '\0');
				stream.read(subMeshName.data(), nameLength);

				// 读取顶点数量
				uint32_t subVertexCount = 0;
				stream.read(reinterpret_cast<char*>(&subVertexCount), sizeof(uint32_t));

				// 读取顶点数据
				std::vector<Vertex> subVertices(subVertexCount);
				stream.read(reinterpret_cast<char*>(subVertices.data()), sizeof(Vertex) * subVertexCount);
				vertices.insert(vertices.end(), subVertices.begin(), subVertices.end());

				// 读取索引数量
				uint32_t subIndexCount = 0;
				stream.read(reinterpret_cast<char*>(&subIndexCount), sizeof(uint32_t));

				// 读取索引数据
				std::vector<uint32_t> subIndices(subIndexCount);
				stream.read(reinterpret_cast<char*>(subIndices.data()), sizeof(uint32_t) * subIndexCount);
				indices.insert(indices.end(), subIndices.begin(), subIndices.end());

				// 存储子网格信息，名称
				mesh->subMeshes.emplace_back(indexOffset, subIndexCount);
				mesh->subMeshNames.emplace_back(subMeshName);

				indexOffset += subIndexCount;
			}

			if (!mesh->subMeshes.empty())
			{
				mesh->subMeshes.pop_back();
			}

			if (!mesh->subMeshNames.empty())
			{
				mesh->subMeshNames.pop_back();
			}

			// bounding box
			CalMeshAABBBound(vertices, mesh->max, mesh->min);

			// gl vao vbo ebo
			LoadMeshGL(vertices, indices, mesh);

			stream.close();
			return mesh;
		}
	}

	//log : file error
	return std::shared_ptr<Mesh>();
}

void Mrk::MeshHut::LoadMeshGL(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Mesh> mesh)
{
	static_assert(sizeof(Vertex) == 8 * sizeof(float), "Vertex struct size mismatch");

	if (mesh)
	{
		GLuint vao, vbo, ebo;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)offsetof(Vertex, texcoord));
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		mesh->vao = vao;
		mesh->vbo = vbo;
		mesh->ebo = ebo;

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL Mesh Error: " << err << std::endl;
		}
	}
}