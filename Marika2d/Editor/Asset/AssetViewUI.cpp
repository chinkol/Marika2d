#include "AssetViewUI.h"

#include "Common/Utility/Utility.h"

Mrk::Editor::AssetNode::AssetNode(const std::filesystem::path& p) :
    name(p.filename().string()),
    path(p.relative_path().make_preferred().string()),
    isDirectory(std::filesystem::is_directory(p))
{
}

void Mrk::Editor::AssetNode::LoadChildren()
{
    if (!isDirectory) return;

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        children.emplace_back(entry.path());
        if (children.back().isDirectory)
        {
            children.back().LoadChildren(); // 递归加载子目录
        }
    }
}

void Mrk::Editor::AssetViewUI::Init()
{
    root = AssetNode(ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
    root.LoadChildren();
}

void Mrk::Editor::AssetViewUI::Draw()
{
    ImGui::Begin("AssetView", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    DrawNode(root);

    ImGui::End();
}

void Mrk::Editor::AssetViewUI::DrawNode(AssetNode& node)
{
    if (node.isDirectory)
    {
        if (ImGui::TreeNodeEx(Mrk::Utility::GBKToUTF8(node.name).c_str()))
        {
            for (auto& child : node.children)
            {
                DrawNode(child);
            }
            ImGui::TreePop();
        }
    }
    else
    {
        ImGui::TreeNodeEx(Mrk::Utility::GBKToUTF8(node.name).c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            auto text = node.path.c_str();
            ImGui::SetDragDropPayload("FILEPATH", text, strlen(text) + 1); // 发送文本
            ImGui::Text(Mrk::Utility::GBKToUTF8(node.name).c_str());
            ImGui::EndDragDropSource();
        }
    }
}
