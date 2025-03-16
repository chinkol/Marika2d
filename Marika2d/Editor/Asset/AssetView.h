#pragma once

#include "Editor/Plugin/Plugin.h"

#include <vector>
#include <string>
#include <filesystem>

namespace Mrk::Editor
{
    struct AssetNode
    {
        std::string name;
        std::string path;
        bool isDirectory = true;
        std::vector<AssetNode> children;

        AssetNode() = default;
        AssetNode(const std::filesystem::path& p);
        void LoadChildren();
    };

#ifndef MRK_ASSETNOEW_BEHAVIOR
#define MRK_ASSETNOEW_BEHAVIOR(x, y) static inline bool _mrk_macro_asset_behavior_##x##_register_ = [](){ AssetNodeBehaviorHut::Register<x>(y); return true; }();
#endif // !MRK_ASSETNOEW_BEHAVIOR

    class IAssertNodeBehavior;

    class AssetNodeBehaviorHut : public Singleton<AssetNodeBehaviorHut>
    {
        MRK_SINGLETON(AssetNodeBehaviorHut)
    public:
        template<typename T>
        static void Register(std::string_view suffix);
        static std::map<std::string, std::function<void()>> GetBehaviors(std::string_view suffix);
    private:
        std::map<std::string, std::function<IAssertNodeBehavior*()>> ctors;
    };

    class IAssertNodeBehavior 
    {
        friend class AssetNodeBehaviorHut;
    public:
        virtual std::map<std::string, std::function<void()>> MakeBehaviors() = 0;
    private:
    };

    class AssertNodeBehavior_mmsh : public IAssertNodeBehavior
    {
        MRK_ASSETNOEW_BEHAVIOR(AssertNodeBehavior_mmsh, ".mmsh")
    public:
        virtual std::map<std::string, std::function<void()>> MakeBehaviors() override;
    };

    class AssetViewUI : public IPlugin
    {
        MRK_PLUGIN(AssetViewUI)
    public:
        void Init();
        void Draw();
    private:
        void DrawNode(AssetNode& node);
        void MouseClick(AssetNode& node);
    private:
        AssetNode root;
    };
}

namespace Mrk::Editor
{
    template<typename T>
    inline void AssetNodeBehaviorHut::Register(std::string_view suffix)
    {
        static_assert(std::is_base_of_v<IAssertNodeBehavior, T>, "T is not AssetNodeBehavior");

        MRK_INSTANCE_REF;

        instance.ctors.try_emplace(suffix.data(), []() {
            return new T();
            });
    }
}