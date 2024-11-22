#pragma once

#include "Common/Singleton/Singleton.h"
#include "Common/Utility/Utility.h"

#include "Core/GameObject/GameObject.h"

#include "Third/assimp/Importer.hpp"
#include "Third/assimp/scene.h"
#include "Third/assimp/postprocess.h"

#include <string>
#include <filesystem>

#ifndef MRK_MODEL_IMPORTER
#define MRK_MODEL_IMPORTER(x) virtual inline std::string_view GetExtension() override { return #x; } 
#endif // !MRK_MODEL_IMPORTER

namespace Mrk
{
	class AssetSystem : public Singleton<AssetSystem>
	{
	public:
		static std::shared_ptr<GameObject> ImportModel()
		{

		}
	private:
		AssetSystem() = default;
	};

	class IModelImporter
	{
	public:
		virtual std::string_view GetExtension() = 0;
		virtual void Import(std::string_view path) = 0;
	};

	class FbxImporter : public IModelImporter
	{
		MRK_MODEL_IMPORTER(fbx)
	public:
		virtual void Import(std::string_view path) override
		{
			if (std::filesystem::exists(path))
			{
				auto utf8Path = Mrk::Utility::GBKToUTF8(path);
				aiScene = importer.ReadFile(utf8Path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
				if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
					throw;
				ProcessNode(aiScene->mRootNode);
			}
		}
	private:
		void ProcessNode(aiNode* aiNode)
		{
			if (aiNode->mNumMeshes > 0) // has mesh(es)
			{
				//mesh
				/*auto mesh = std::make_shared<Mesh>();
				mesh->SetName(path + "/" + aiNode->mName.C_Str());
				if (RenderResourcePool::Push(mesh))
					FillMeshFromAiNode(mesh, aiNode);*/

				//material
				for (unsigned int i = 0; i < aiNode->mNumMeshes; i++)
				{
					aiMesh* aiMesh = aiScene->mMeshes[aiNode->mMeshes[i]];
					aiMaterial* aiMat = aiScene->mMaterials[aiMesh->mMaterialIndex];
					//ProcessMaterial(aiMat);
				}
			}

			//child nodes
			for (unsigned int i = 0; i < aiNode->mNumChildren; i++)
			{
				ProcessNode(aiNode->mChildren[i]);
			}
		}
		void ProcessMaterial(aiMaterial* aiMat)
		{

		}
	private:
		Assimp::Importer importer;
		const aiScene* aiScene;
	};

}

