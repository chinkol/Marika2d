#include "PluginTexureMergeUI.h"

#include "Core/Render/Texture/Texture.h"

#include "Third/imnodes/imnodes.h"

void Mrk::PluginTexureMergeUI::SetMesh(std::shared_ptr<Mesh> mesh)
{
    this->mesh = mesh;
}

void Mrk::PluginTexureMergeUI::Draw()
{
    ImNodesStyle& style = ImNodes::GetStyle();
    style.NodePadding = ImVec2(0, 0);  // 去除节点内边距

    ImGui::Begin("Texture Merge");
    {
        ImNodes::BeginNodeEditor();
        {
            int index = 0;

            auto& textures = TextureHut::GetTextures();
            for (auto& [_, tex] : textures)
            {
                ImNodes::BeginNode(index);
                {
                    ImVec2 tex_size = ImVec2((float)tex->GetWidth() / 16, (float)tex->GetHeight() / 16);

                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                    ImGui::Image((ImTextureID)tex->GetID(), tex_size);
                    ImGui::PopStyleVar();
                }
                ImNodes::EndNode();
                index++;
            }
        }
        ImNodes::EndNodeEditor();
    }
    ImGui::End();
}
