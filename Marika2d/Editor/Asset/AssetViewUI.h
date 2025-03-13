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

    class AssetViewUI : public IPlugin
    {
        MRK_PLUGIN(AssetViewUI)
    public:
        void Init();
        void Draw();
    private:
        void DrawNode(AssetNode& node);
    private:
        AssetNode root;
    };
}