#include "AssetView.h"

#include "Editor/Plugin/PluginPathSelectDlg/PluginPathSelectDlg.h"
#include "Editor/Plugin/PluginInputDlg/PluginInputDlg.h"

#include "Common/Utility/Utility.h"

#include "Core/Render/Material/Material.h"

Mrk::Editor::AssetUINode::AssetUINode(const std::filesystem::path& p) :
    name(p.filename().string()),
    path(p.relative_path().make_preferred().string()),
    isDirectory(std::filesystem::is_directory(p))
{
}

void Mrk::Editor::AssetUINode::LoadChildren()
{
    if (!isDirectory) return;

    children.clear();
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        children.emplace_back(entry.path()).parent = this;
        if (children.back().isDirectory)
        {
            children.back().LoadChildren(); // 递归加载子目录
        }
    }
}

void Mrk::Editor::AssetUINode::Delete()
{
    if (std::filesystem::exists(path))
    {
        if (isDirectory)
        {
            std::filesystem::remove_all(path);
        }
        else
        {
            std::filesystem::remove(path);
        }
    }

    if (parent)
    {
        parent->LoadChildren();
    }
}

void Mrk::Editor::PluginAssetViewUI::Init()
{
    root = AssetUINode(ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
    root.LoadChildren();
}

void Mrk::Editor::PluginAssetViewUI::Draw()
{
    ImGui::Begin("AssetView", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    DrawNode(root);

    ImGui::End();
}

void Mrk::Editor::PluginAssetViewUI::DrawNode(AssetUINode& node)
{
    if (node.isDirectory)
    {
        if (dirtyDirs.contains(node.path))
        {
            node.LoadChildren();
            dirtyDirs.erase(node.path);
        }

        bool isOpen = ImGui::TreeNodeEx(Mrk::Utility::GBKToUTF8(node.name).c_str());

        MouseRightClick(node);
        MouseLeftClick(node);
        DragDrop(node);

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
        DragDrop(node);
    }
}

void Mrk::Editor::PluginAssetViewUI::MouseRightClick(AssetUINode& node)
{
    if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(node.path.c_str());
    }

    if (ImGui::BeginPopup(node.path.c_str()))
    {
        std::filesystem::path path = node.path;

        // default behaviors
        auto defaultBehaviors = MakeDefaultRightClickBehaviors(node);
        for (auto& [name, behavior] : defaultBehaviors)
        {
            if (ImGui::MenuItem(name.c_str()))
            {
                behavior(node);
            }
        }

        // custom behaviors
        auto behaviors = AssetUINodeBehaviorHut::GetRightClickBehaviors(path.extension().string());
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

void Mrk::Editor::PluginAssetViewUI::MouseLeftClick(AssetUINode& node)
{
    if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        std::filesystem::path path = node.path;

        if (auto behavior = AssetUINodeBehaviorHut::GetLeftClickBehavior(path.extension().string()))
        {
            behavior(node);
        }
    }
}

void Mrk::Editor::PluginAssetViewUI::DragDrop(AssetUINode& node)
{
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        auto text = node.path.c_str();
        ImGui::SetDragDropPayload("FILEPATH", text, strlen(text) + 1); // 发送路径
        ImGui::Text(Mrk::Utility::GBKToUTF8(node.name).c_str());
        ImGui::EndDragDropSource();
    }

    if (node.isDirectory && ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILEPATH"))
        {
            if (payload->DataSize > 0)
            {
                char buffer[256];
                strncpy_s(buffer, static_cast<const char*>(payload->Data), sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';

                std::filesystem::path fromPath(buffer);
                std::filesystem::path toPath = std::filesystem::path(node.path) / fromPath.filename();
                if (std::filesystem::exists(fromPath))
                {
                    std::filesystem::rename(fromPath, toPath);

                    dirtyDirs.emplace(fromPath.parent_path().string());
                    dirtyDirs.emplace(node.path);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
}

std::map<std::string, std::function<void(Mrk::Editor::AssetUINode&)>> Mrk::Editor::PluginAssetViewUI::MakeDefaultRightClickBehaviors(AssetUINode& node)
{
    std::map<std::string, std::function<void(Mrk::Editor::AssetUINode&)>> res;

    if (node.isDirectory)
    {
        res.emplace("Refresh", [this](AssetUINode& node) {
            dirtyDirs.emplace(node.path);
            });
    }

    if (node.parent)    // 根目录跳过
    {
        res.emplace("Delete", [](AssetUINode& node) {
            node.Delete();  //TODO:
            });
    }

    if (node.isDirectory)
    {
        res.emplace("Create New Folder", [this](AssetUINode& node) {
            PluginInputDlg::GetInstance()->GetString([this, node](const std::string& str) {
                std::filesystem::path path = node.path;
                path /= str;
                if (!std::filesystem::exists(path))
                {
                    (void)std::filesystem::create_directories(path);
                    dirtyDirs.emplace(node.path);
                }
                });
            });
    }

    return res;
}

std::map<std::string, std::function<void(Mrk::Editor::AssetUINode&)>> Mrk::Editor::AssetUINodeBehaviorHut::GetRightClickBehaviors(std::string_view suffix)
{
    MRK_INSTANCE_REF;

    auto ret = instance.ctors.find(pystr::lower(suffix.data()));
    if (ret != instance.ctors.end())
    {
        return std::unique_ptr<IAssertUINodeBehavior>(ret->second())->MakeRightClickBehaviors();
    }

    return std::map<std::string, std::function<void(Mrk::Editor::AssetUINode&)>>();
}

std::function<void(Mrk::Editor::AssetUINode&)> Mrk::Editor::AssetUINodeBehaviorHut::GetLeftClickBehavior(std::string_view suffix)
{
    MRK_INSTANCE_REF;

    auto ret = instance.ctors.find(pystr::lower(suffix.data()));
    if (ret != instance.ctors.end())
    {
        return std::unique_ptr<IAssertUINodeBehavior>(ret->second())->MakeLeftClickBehavior();
    }

    return std::function<void(Mrk::Editor::AssetUINode&)>();
}

std::map<std::string, std::function<void(Mrk::Editor::AssetUINode&)>> Mrk::Editor::AssetUINodeBehavior_mmsh::MakeRightClickBehaviors()
{
    return {
        {"This is a mmsh", [](Mrk::Editor::AssetUINode& node) {
            std::cout << node.path << std::endl;
        }}
    };
}

std::map<std::string, std::function<void(Mrk::Editor::AssetUINode&)>> Mrk::Editor::IAssertUINodeBehavior::MakeRightClickBehaviors()
{
    return std::map<std::string, std::function<void(Mrk::Editor::AssetUINode&)>>();
}

std::function<void(Mrk::Editor::AssetUINode&)> Mrk::Editor::IAssertUINodeBehavior::MakeLeftClickBehavior()
{
    return std::function<void(Mrk::Editor::AssetUINode&)>();
}

std::map<std::string, std::function<void(Mrk::Editor::AssetUINode&)>> Mrk::Editor::AssetUINodeBehavior_vert::MakeRightClickBehaviors()
{
    return {
         {"Create (.mmat) File", [](Mrk::Editor::AssetUINode& node) {
             if (auto sp = Mrk::ShaderProgramHut::GetShaderProgram(node.path))
             {
                 PluginPathSelectDlg::GetInstance()->SelectNewFile([sp](const std::filesystem::path& path) {
                     auto mat = sp->CreateMaterial();
                     Json::Document jdoc;
                     Mrk::Utility::SaveJson(mat->ToJson(jdoc.GetAllocator()), path.string());
                     });
             }
         }}
    };
}

std::function<void(Mrk::Editor::AssetUINode&)> Mrk::Editor::AssetUINodeBehavior_mmat::MakeLeftClickBehavior()
{
    return [](Mrk::Editor::AssetUINode& node) {
        Mrk::PluginMaterialEditUI::GetInstance()->SetSelectedMaterialFile(node.path);
        };
}
