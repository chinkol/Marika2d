#pragma once

#include "Common/Singleton/Singleton.h"
#include "Common/Utility/Utility.h"

#include "Core/GameObject/GameObject.h"

#include "Third/assimp/Importer.hpp"
#include "Third/assimp/scene.h"
#include "Third/assimp/postprocess.h"

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
return #y;																					\
}																							\
static inline std::string_view StaticGetExtension() {										\
return #y;																					\
}																							\
static inline bool _mrk_macro_##x##_asset_importer_register_ = [](){						\
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
	private:
		AssetSystem() = default;
	private:
		std::map<std::string, std::function<IAssetlImporter*()>> creators;
	};

	class IAssetlImporter
	{
	public:
		virtual std::string_view GetExtension() = 0;
		virtual void Import(std::string_view path) = 0;
	};

	class FbxImporter : public IAssetlImporter
	{
		MRK_ASSET_IMPORTER(FbxImporter, fbx)
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

