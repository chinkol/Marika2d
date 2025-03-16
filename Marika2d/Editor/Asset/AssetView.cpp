#include "AssetView.h"

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
            MouseClick(node);

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

        MouseClick(node);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            auto text = node.path.c_str();
            ImGui::SetDragDropPayload("FILEPATH", text, strlen(text) + 1); // 发送文本
            ImGui::Text(Mrk::Utility::GBKToUTF8(node.name).c_str());
            ImGui::EndDragDropSource();
        }
    }
}

void Mrk::Editor::AssetViewUI::MouseClick(AssetNode& node)
{
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) 
    {
        ImGui::OpenPopup("AssetRightPopup");
    }

    if (ImGui::BeginPopup("AssetRightPopup")) 
    {
        std::filesystem::path path = node.path;

        auto behaviors = AssetNodeBehaviorHut::GetBehaviors(path.extension().string());
        for (auto& [name, behavior] : behaviors)
        {
            if (ImGui::MenuItem(name.c_str())) 
            {
                behavior();
            }
        }

        ImGui::EndPopup();
    }
}

std::map<std::string, std::function<void()>> Mrk::Editor::AssetNodeBehaviorHut::GetBehaviors(std::string_view suffix)
{
    MRK_INSTANCE_REF;

    auto ret = instance.ctors.find(suffix.data());
    if (ret != instance.ctors.end())
    {
        return std::unique_ptr<IAssertNodeBehavior>(ret->second())->MakeBehaviors();
    }
    
    return std::map<std::string, std::function<void()>>();
}

std::map<std::string, std::function<void()>> Mrk::Editor::AssertNodeBehavior_mmsh::MakeBehaviors()
{
    return {
        {"This is a mmsh", []() {
            std::cout << "This is a mmsh" << std::endl;
        }}
    };
}
