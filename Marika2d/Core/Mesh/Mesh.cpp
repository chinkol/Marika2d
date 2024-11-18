#include "Mesh.h"

inline Mrk::Vertex::Vertex() :
	position(0, 0, 0),
	normal(0, 0, 0),
	texcoord(0, 0)
{
}

inline Mrk::Vertex::Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texcoord) :
	position(position),
	normal(normal),
	texcoord(texcoord)
{
}

inline Mrk::Vertex::Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty) :
	position(px, py, pz),
	normal(nx, ny, nz),
	texcoord(tx, ty)
{
}

Mrk::Mesh::Mesh()
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
