#pragma once

#include "Editor/Plugin/Plugin.h"

#include "Third/pystr/pystr.h"

#include <vector>
#include <string>
#include <filesystem>
#include <set>

#ifndef MRK_ASSETNOEW_BEHAVIOR
#define MRK_ASSETNOEW_BEHAVIOR(x, y) static inline bool _mrk_macro_asset_behavior_##x##_register_ = [](){ AssetUINodeBehaviorHut::Register<x>(y); return true; }();
#endif // !MRK_ASSETNOEW_BEHAVIOR

namespace Mrk::Editor
{
    struct AssetUINode
    {
        std::string name;
        std::string path;
        bool isDirectory = true;
        std::vector<AssetUINode> children;
        AssetUINode* parent = nullptr;;

        AssetUINode() = default;
        AssetUINode(const std::filesystem::path& p);
        void LoadChildren();
        void Delete();
    };

    class IAssertUINodeBehavior;

    class AssetUINodeBehaviorHut : public Singleton<AssetUINodeBehaviorHut>
    {
        MRK_SINGLETON(AssetUINodeBehaviorHut)
    public:
        template<typename T>
        static void Register(std::string_view suffix);
        static std::map<std::string, std::function<void(AssetUINode&)>> GetRightClickBehaviors(std::string_view suffix);
        static std::function<void(Mrk::Editor::AssetUINode&)> GetLeftClickBehavior(std::string_view suffix);
    private:
        std::map<std::string, std::function<IAssertUINodeBehavior* ()>> ctors;
    };

    class IAssertUINodeBehavior
    {
        friend class AssetUINodeBehaviorHut;
    public:
        virtual std::map<std::string, std::function<void(AssetUINode&)>> MakeRightClickBehaviors();
        virtual std::function<void(AssetUINode&)> MakeLeftClickBehavior();
    private:
    };

    class AssetUINodeBehavior_mmsh : public IAssertUINodeBehavior
    {
        MRK_ASSETNOEW_BEHAVIOR(AssetUINodeBehavior_mmsh, ".mmsh")
    public:
        virtual std::map<std::string, std::function<void(AssetUINode&)>> MakeRightClickBehaviors() override;
    };

    class AssetUINodeBehavior_mmat : public IAssertUINodeBehavior
    {
        MRK_ASSETNOEW_BEHAVIOR(AssetUINodeBehavior_mmat, ".mmat")
    public:
        virtual std::function<void(AssetUINode&)> MakeLeftClickBehavior() override;
    };

    class AssetUINodeBehavior_vert : public IAssertUINodeBehavior
    {
        MRK_ASSETNOEW_BEHAVIOR(AssetUINodeBehavior_vert, ".vert")
    public:
        virtual std::map<std::string, std::function<void(AssetUINode&)>> MakeRightClickBehaviors();
    };

    class AssetUINodeBehavior_frag : public AssetUINodeBehavior_vert
    {
        MRK_ASSETNOEW_BEHAVIOR(AssetUINodeBehavior_frag, ".frag")
    };

    class PluginAssetTreeUI : public IPlugin
    {
        MRK_PLUGIN(PluginAssetTreeUI)
    public:
        void Init();
        void Draw();
        const std::string& GetSelected();

    private:
        void DrawNode(AssetUINode& node);
        void MouseRightClick(AssetUINode& node);
        void MouseLeftClick(AssetUINode& node);
        void DragDrop(AssetUINode& node);
    private:
        std::map<std::string, std::function<void(AssetUINode&)>> MakeDefaultRightClickBehaviors(AssetUINode& node);
    private:
        AssetUINode root;
        std::set<std::string> dirtyDirs;
        std::string selected;
    };

    class PluginAssetViewUI : public IPlugin
    {
        MRK_PLUGIN(PluginAssetViewUI)
    public:
        void Draw();
    };
}

namespace Mrk::Editor
{
    template<typename T>
    inline void AssetUINodeBehaviorHut::Register(std::string_view suffix)
    {
        static_assert(std::is_base_of_v<IAssertUINodeBehavior, T>, "T is not AssetNodeBehavior");

        MRK_INSTANCE_REF;

        instance.ctors.try_emplace(pystr::lower(suffix.data()), []() {
            return new T();
            });
    }
}