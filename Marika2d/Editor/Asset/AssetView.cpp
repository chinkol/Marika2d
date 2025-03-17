#include "AssetView.h"

#include "Common/Utility/Utility.h"

#include "Third/nfd/nfd.h"

#include "Core/Render/Material/Material.h"

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
        bool isOpen = ImGui::TreeNodeEx(Mrk::Utility::GBKToUTF8(node.name).c_str());

        MouseRightClick(node);
        MouseLeftClick(node);

        if (isOpen)
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

        MouseRightClick(node);
        MouseLeftClick(node);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            auto text = node.path.c_str();
            ImGui::SetDragDropPayload("FILEPATH", text, strlen(text) + 1); // 发送文本
            ImGui::Text(Mrk::Utility::GBKToUTF8(node.name).c_str());
            ImGui::EndDragDropSource();
        }
    }
}

void Mrk::Editor::AssetViewUI::MouseRightClick(AssetNode& node)
{
    if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(node.path.c_str());
    }

    if (ImGui::BeginPopup(node.path.c_str()))
    {
        std::filesystem::path path = node.path;

        auto behaviors = AssetNodeBehaviorHut::GetRightClickBehaviors(path.extension().string());
        for (auto& [name, behavior] : behaviors)
        {
            if (ImGui::MenuItem(name.c_str())) 
            {
                behavior(node);
            }
        }

        ImGui::EndPopup();
    }
}

void Mrk::Editor::AssetViewUI::MouseLeftClick(AssetNode& node)
{
    if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        std::filesystem::path path = node.path;

        auto behaviors = AssetNodeBehaviorHut::GetLeftClickBehaviors(path.extension().string());
        for (auto& [name, behavior] : behaviors)
        {
            behavior(node);
        }
    }
}

std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>> Mrk::Editor::AssetNodeBehaviorHut::GetRightClickBehaviors(std::string_view suffix)
{
    MRK_INSTANCE_REF;

    auto ret = instance.ctors.find(suffix.data());
    if (ret != instance.ctors.end())
    {
        return std::unique_ptr<IAssertNodeBehavior>(ret->second())->MakeRightClickBehaviors();
    }
    
    return std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>>();
}

std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>> Mrk::Editor::AssetNodeBehaviorHut::GetLeftClickBehaviors(std::string_view suffix)
{
    MRK_INSTANCE_REF;

    auto ret = instance.ctors.find(suffix.data());
    if (ret != instance.ctors.end())
    {
        return std::unique_ptr<IAssertNodeBehavior>(ret->second())->MakeLeftClickBehaviors();
    }

    return std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>>();
}

std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>> Mrk::Editor::AssetNodeBehavior_mmsh::MakeRightClickBehaviors()
{
    return {
        {"This is a mmsh", [](const Mrk::Editor::AssetNode& node) {
            std::cout << node.path << std::endl;
        }}
    };
}

std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>> Mrk::Editor::IAssertNodeBehavior::MakeRightClickBehaviors()
{
    return std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>>();
}

std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>> Mrk::Editor::IAssertNodeBehavior::MakeLeftClickBehaviors()
{
    return std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>>();
}

std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>> Mrk::Editor::AssetNodeBehavior_vert::MakeRightClickBehaviors()
{
    return {
         {"Create (.mmat) File", [](const Mrk::Editor::AssetNode& node) {
             if (auto sp = Mrk::ShaderProgramHut::GetShaderProgram(node.path))
             {
                 nfdchar_t* to;

                 if (NFD_SaveDialog("mmat", NULL, &to) == NFD_OKAY)
                 {
                     auto mat = sp->CreateMaterial();
                     Json::Document jdoc;
                     Mrk::Utility::SaveJson(mat->ToJson(jdoc.GetAllocator()), Mrk::Utility::UFT8ToGBK(to));
                 }
             }
         }}
    };
}

std::map<std::string, std::function<void(const Mrk::Editor::AssetNode&)>> Mrk::Editor::AssetNodeBehavior_mmat::MakeLeftClickBehaviors()
{
    return {
         {"Create (.mmat) File", [](const Mrk::Editor::AssetNode& node) {
             Mrk::PluginMaterialEditor::GetInstance()->SetSelectedMaterialFile(node.path);
         }}
    };
}
