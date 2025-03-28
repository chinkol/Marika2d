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
friend class AssetSys;																	\
private:																					\
virtual inline std::string_view GetExtension() override {									\
return "."#y;																				\
}																							\
static inline std::string_view StaticGetExtension() {										\
return "."#y;																				\
}																							\
static inline bool _mrk_macro_##y##_asset_importer_register_ = [](){						\
AssetSys::RegisterImporter<x>();															\
return true;																				\
}();
#endif // !MRK_ASSET_IMPORTER

namespace Mrk
{
	class IAssetlImporter;
	class GameObject;
	class MeshRenderer;

	class AssetSys : public Singleton<AssetSys>
	{
		MRK_SINGLETON(AssetSys)
	public:
		template<typename T> static void RegisterImporter();
		static const std::vector<std::string>& GetImportSupports();
		static void Import(const std::filesystem::path& from, const std::filesystem::path& to);
	private:
		AssetSys() = default;
	private:
		std::map<std::string, std::function<IAssetlImporter* ()>> creators;
	};

	class IAssetlImporter
	{
	public:
		virtual ~IAssetlImporter() = default;
		virtual std::string_view GetExtension() = 0;
		virtual void Import(const std::filesystem::path& from, const std::filesystem::path& to) = 0;
	};

	class AssetUtility
	{
	public:
		static void MeshDataToLocalFile(const std::vector<std::string> names, const std::vector<std::vector<Vertex>>& vertices, const std::vector<std::vector<uint32_t>>& indices, const std::filesystem::path& filename);
		static void MakePrefab(std::shared_ptr<GameObject> gameObject, const std::filesystem::path& to);
	private:
		AssetUtility() = default;
	};

	class AssimpAssetImporter : public IAssetlImporter
	{
		struct subMeshInfo
		{
			std::string name;
			aiMesh* aiMesh;
		};
	public:
		virtual ~AssimpAssetImporter() override;
		virtual void Import(const std::filesystem::path& from, const std::filesystem::path& to) override;
	protected:
		void ProcessNode(aiNode* aiNode, const std::filesystem::path& from, const std::filesystem::path& to, std::shared_ptr<GameObject> objNode);
		void ProcessMaterial(aiMaterial* aiMat, const std::filesystem::path& from, const std::filesystem::path& to, std::shared_ptr<MeshRenderer> meshRenderer);
		void ProcessTexture(const std::filesystem::path& from, const std::filesystem::path& to);
		void ProcessMesh(std::vector<subMeshInfo> aiMesh, const std::filesystem::path& to);
		void ProcessSubMesh(aiMesh* aiMesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	protected:
		Assimp::Importer importer;
		const aiScene* aiScene;
	};

	class FbxImporter : public AssimpAssetImporter
	{
		MRK_ASSET_IMPORTER(FbxImporter, fbx)
	};

	class PmxImporter : public AssimpAssetImporter
	{
		MRK_ASSET_IMPORTER(PmxImporter, pmx)
	};
}

namespace Mrk
{
	template<typename T>
	inline void AssetSys::RegisterImporter()
	{
		static_assert(std::is_base_of_v<IAssetlImporter, T>, "T Is Not A IAssetlImporter !");
		if (!Instance().creators.try_emplace(T::StaticGetExtension().data(), []() {
			return new T(); }).second)
		{
			throw; // repeat ?
		}
	}
}