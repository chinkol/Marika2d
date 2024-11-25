#pragma once

#include "Common/Singleton/Singleton.h"
#include "Common/Utility/Utility.h"

#include "Core/Mesh/Mesh.h"
#include "Core/GameObject/GameObject.h"

#include "Third/assimp/Importer.hpp"
#include "Third/assimp/scene.h"
#include "Third/assimp/postprocess.h"

#include <span>
#include <fstream>
#include <vector>
#include <ranges>
#include <string>
#include <filesystem>
#include <map>

#ifndef MRK_ASSET_IMPORTER
#define MRK_ASSET_IMPORTER(x, y) struct y{};												\
friend class AssetSystem;																	\
private:																					\
virtual inline std::string_view GetExtension() override {									\
return "."#y;																				\
}																							\
static inline std::string_view StaticGetExtension() {										\
return "."#y;																				\
}																							\
static inline bool _mrk_macro_##y##_asset_importer_register_ = [](){						\
AssetSystem::RegisterImporter<x>();															\
return true;																				\
}();
#endif // !MRK_ASSET_IMPORTER

namespace Mrk
{
	class IAssetlImporter;

	class AssetSystem : public Singleton<AssetSystem>
	{
		MRK_SINGLETON(AssetSystem)
	public:
		template<typename T> static void RegisterImporter();
		static const std::vector<std::string>& GetImportSupports();
		static void Import(const std::filesystem::path& from, const std::filesystem::path& to);
	private:
		AssetSystem() = default;
	private:
		std::map<std::string, std::function<IAssetlImporter*()>> creators;
	};

	class IAssetlImporter
	{
	public:
		virtual std::string_view GetExtension() = 0;
		virtual void Import(const std::filesystem::path& from, const std::filesystem::path& to) = 0;
	};

	class AssetUtility
	{
	public:
		static void MeshToLocalFile(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::string_view filename)
		{
			std::ofstream file(filename.data(), std::ios::binary);
			if (!file.is_open()) {
				std::cerr << "Cant Open File : " << filename << std::endl;
				return;
			}

			// 写入 80 字节的文件头
			char header[80] = {};
			std::memcpy(header, "Marika Engine Mesh", std::strlen("Marika Engine Mesh"));
			file.write(header, 80);

			//写入顶点数量
			auto vertexCount = vertices.size();
			file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(uint32_t));

			//写入顶点
			for (auto& vertex : vertices)
			{
				file.write(reinterpret_cast<const char*>(&vertex.normal), sizeof(vertex.normal));
				file.write(reinterpret_cast<const char*>(&vertex.position), sizeof(vertex.position));
				file.write(reinterpret_cast<const char*>(&vertex.texcoord), sizeof(vertex.texcoord));
			}

			// 写入索引
			auto indexCount = indices.size();
			file.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));

			for (auto& index : indices)
			{
				file.write(reinterpret_cast<const char*>(&index), sizeof(index));
			}

			file.close();
		}
	private:
		AssetUtility() = default;
	};

	class FbxImporter : public IAssetlImporter
	{
		MRK_ASSET_IMPORTER(FbxImporter, fbx)
	public:
		virtual void Import(const std::filesystem::path& from, const std::filesystem::path& to) override
		{
			if (std::filesystem::exists(from))
			{
				aiScene = importer.ReadFile(Utility::GBKToUTF8(from.string()), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
				if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
					throw;	// error file
				ProcessNode(aiScene->mRootNode);
			}
			else
			{
				throw; // not exists
			}
		}
	private:
		void ProcessNode(aiNode* aiNode)
		{
			if (aiNode->mNumMeshes > 0)
			{
				//std::span<aiMesh*>(aiScene->mMeshes, aiNode->mNumMeshes);
				for (unsigned int i = 0; i < aiNode->mNumMeshes; i++)
				{
					auto aiMesh = aiScene->mMeshes[aiNode->mMeshes[i]];
					auto aiMat = aiScene->mMaterials[aiMesh->mMaterialIndex];
					ProcessMesh(aiMesh);
					ProcessMaterial(aiMat);
				}
			}

			// child nodes
			for (unsigned int i = 0; i < aiNode->mNumChildren; i++)
			{
				ProcessNode(aiNode->mChildren[i]);
			}
		}
		void ProcessMaterial(aiMaterial* aiMat)
		{

		}
		void ProcessMesh(aiMesh* aiMesh)
		{
			//vertices
			std::vector<Vertex> vertices;
			vertices.reserve(aiMesh->mNumVertices);
			for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
			{
				auto& aiPosition = aiMesh->mVertices[i];
				auto& aiNormal = aiMesh->mNormals[i];
				auto& aiTexCoord = aiMesh->mTextureCoords[0][i];

				auto position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
				auto normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
				auto texCoord = glm::vec2(aiTexCoord.x, aiTexCoord.y);

				vertices.push_back(Vertex(position, normal, texCoord));
			}
			vertices.insert(vertices.end(), vertices.begin(), vertices.end());

			//indices
			std::vector<unsigned int> indices;
			indices.reserve(aiMesh->mNumFaces * 3);
			for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
			{
				auto& face = aiMesh->mFaces[i];
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			AssetUtility::MeshToLocalFile(vertices, indices, "D:/SourceCode/Marika2d/Marika2d/TestProject/test.mem");
		}
	private:
		Assimp::Importer importer;
		const aiScene* aiScene;
	};
}

namespace Mrk
{
	template<typename T>
	inline void AssetSystem::RegisterImporter()
	{
		static_assert(std::is_base_of_v<IAssetlImporter, T>, "T Is Not A IAssetlImporter !");
		if (!Instance().creators.try_emplace(T::StaticGetExtension().data(), []() {
			return new T(); }).second)
		{
			throw; // repeat ?
		}
	}
}

