#include "AssetSystem.h"

#include "Core/GameObject/GameObject.h"
#include "Core/Mesh/MeshRenderer.h"

const std::vector<std::string>& Mrk::AssetSys::GetImportSupports()
{
	static auto supportsView = Instance().creators | std::ranges::views::transform([](const auto& pair) { return pair.first; });
	static auto supports = std::vector<std::string>(supportsView.begin(), supportsView.end());

	return supports;
}

void Mrk::AssetSys::Import(const std::filesystem::path& from, const std::filesystem::path& to)
{
	auto creator = Instance().creators.find(from.extension().string());
	if (creator != Instance().creators.end())
	{
		auto importer = std::unique_ptr<IAssetlImporter>(creator->second());
		importer->Import(from, to);
	}
	else
	{
		throw; //unsupport file type
	}
}

void Mrk::AssetUtility::MeshDataToLocalFile(const std::vector<std::vector<Vertex>>& vertices, const std::vector<uint32_t>& indices, const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path.parent_path()))
	{
		std::filesystem::create_directories(path.parent_path());
	}

	std::ofstream file(path, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Cant Open File : " << path << std::endl;
		return;
	}

	// 写入 80 字节的文件头
	char header[80] = {};
	std::memcpy(header, "Marika Mesh", std::strlen("Marika Mesh"));
	file.write(header, 80);

	// 写入子网格数据

	uint32_t indexOffset = 0;

	for (auto& subVertices : vertices)
	{
		//写入顶点数量
		auto vertexCount = (uint32_t)subVertices.size();
		file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(uint32_t));

		//写入顶点
		for (auto& vertex : subVertices)
		{
			file.write(reinterpret_cast<const char*>(&vertex.normal), sizeof(vertex.normal));
			file.write(reinterpret_cast<const char*>(&vertex.position), sizeof(vertex.position));
			file.write(reinterpret_cast<const char*>(&vertex.texcoord), sizeof(vertex.texcoord));
		}

		// 写入索引
		auto indexCount = subVertices.size() * 3;
		file.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));

		for (auto& index : indices)
		{
			auto realIndex = index + indexOffset;
			file.write(reinterpret_cast<const char*>(&realIndex), sizeof(realIndex));
		}

		indexOffset += indexCount;
	}

	file.close();
}

void Mrk::AssetUtility::MakePrefab(std::shared_ptr<GameObject> gameObject, const std::filesystem::path& to)
{
	assert(to.has_filename());

	if (gameObject)
	{
		if (!std::filesystem::exists(to.parent_path()))
		{
			std::filesystem::create_directories(to.parent_path());
		}

		auto path = to;
		path.replace_extension(".mpre");

		std::ofstream ofstream(path);
		if (!ofstream.is_open())
		{
			//TODO : log
			throw;
		}

		Json::StringBuffer buf;
		Json::PrettyWriter writer(buf);

		Json::Document jdoc;
		auto json = gameObject->ToJson(jdoc.GetAllocator());
		json.Accept(writer);

		ofstream << buf.GetString() << std::endl;
		ofstream.close();
	}
}

void Mrk::FbxImporter::Import(const std::filesystem::path& from, const std::filesystem::path& to)
{
	if (std::filesystem::exists(from))
	{
		aiScene = importer.ReadFile(Utility::GBKToUTF8(from.string()), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
			throw;	// error file

		if (!std::filesystem::exists(to.parent_path()))
		{
			std::filesystem::create_directories(to.parent_path());
		}

		auto objRoot = GameObjectFactory::CreateNew<GameObject>();
		objRoot->SetName(from.filename().replace_extension().string());
		ProcessNode(aiScene->mRootNode, from, to / from.filename().replace_extension(), objRoot);

		AssetUtility::MakePrefab(objRoot, to / from.filename().replace_extension() / from.filename().replace_extension(".mpre"));
	}
	else
	{
		throw; // not exists
	}
}

void Mrk::FbxImporter::ProcessNode(aiNode* aiNode, const std::filesystem::path& from, const std::filesystem::path& to, std::shared_ptr<GameObject> objNode)
{
	// meshes
	if (aiNode->mNumMeshes > 0)
	{
		auto meshRenderer = ComponentFactory::CreateNew<MeshRenderer>();

		std::vector<aiMesh*> aiMesh;
		for (uint32_t i = 0; i < aiNode->mNumMeshes; i++)
		{
			auto aiSubMesh = aiScene->mMeshes[aiNode->mMeshes[i]];
			aiMesh.push_back(aiSubMesh);

			// mat
			auto aiMat = aiScene->mMaterials[aiSubMesh->mMaterialIndex];
			ProcessMaterial(aiMat, from, to / "Materials", meshRenderer);
		}

		auto dump = to / "Meshes" / aiNode->mName.C_Str();	// 转储路径
		dump += MRK_MESH_FILE_EXTENSION;

		ProcessMesh(aiMesh, dump);

		meshRenderer->SetMeshPath(dump.string());
		objNode->AddComponent(meshRenderer);
	}

	// child nodes
	for (uint32_t i = 0; i < aiNode->mNumChildren; i++)
	{
		auto aiSubNode = aiNode->mChildren[i];
		auto subNode = GameObjectFactory::CreateNew<GameObject>();

		ProcessNode(aiSubNode, from, to, subNode);

		subNode->SetName(Utility::UFT8ToGBK(aiSubNode->mName.C_Str()));
		objNode->AddChild(subNode);
	}
}

void Mrk::FbxImporter::ProcessMaterial(aiMaterial* aiMat, const std::filesystem::path& from, const std::filesystem::path& to, std::shared_ptr<MeshRenderer> meshRenderer)
{
	for (uint32_t i = 0; i < aiMat->GetTextureCount(aiTextureType_DIFFUSE); i++)
	{
		aiString aiPath;
		aiMat->GetTexture(aiTextureType_DIFFUSE, i, &aiPath);

		std::filesystem::path path = from.parent_path() / Mrk::Utility::UFT8ToGBK(aiPath.C_Str());

		ProcessTexture(path, to / path.filename());

		//TODO : meshRenderer
	}
}

void Mrk::FbxImporter::ProcessTexture(const std::filesystem::path& from, const std::filesystem::path& to)
{
	if (!std::filesystem::exists(to.parent_path()))
	{
		std::filesystem::create_directories(to.parent_path());
	}

	if (std::filesystem::exists(from))
	{
		std::filesystem::copy(from, to, std::filesystem::copy_options::overwrite_existing);
	}
}

void Mrk::FbxImporter::ProcessMesh(std::vector<aiMesh*> aiMesh, const std::filesystem::path& to)
{
	std::vector<std::vector<Vertex>> vertices;
	std::vector<uint32_t> indices;

	for (auto aiSubMesh : aiMesh)
	{
		std::vector<Vertex> subVertices;
		ProcessSubMesh(aiSubMesh, subVertices, indices);
		vertices.push_back(subVertices);
	}

	AssetUtility::MeshDataToLocalFile(vertices, indices, to);
}

void Mrk::FbxImporter::ProcessSubMesh(aiMesh* aiSubMesh, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	//vertices
	vertices.reserve(aiSubMesh->mNumVertices);
	for (uint32_t i = 0; i < aiSubMesh->mNumVertices; i++)
	{
		auto& aiPosition = aiSubMesh->mVertices[i];
		auto& aiNormal = aiSubMesh->mNormals[i];
		auto& aiTexCoord = aiSubMesh->mTextureCoords[0][i];

		auto position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
		auto normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
		auto texCoord = glm::vec2(aiTexCoord.x, aiTexCoord.y);

		vertices.push_back(Vertex(position, normal, texCoord));
	}
	vertices.insert(vertices.end(), vertices.begin(), vertices.end());

	//indices
	indices.reserve(aiSubMesh->mNumFaces * 3);
	for (uint32_t i = 0; i < aiSubMesh->mNumFaces; i++)
	{
		auto& face = aiSubMesh->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
}