#include "MeshRenderer.h"

#include "Core/Render/Render.h"
#include "Core/GameObject/GameObject.h"

#include <algorithm>

const std::string& Mrk::MeshRenderer::GetMeshPath()
{
    return meshPath;
}

void Mrk::MeshRenderer::SetMeshPath(const std::string& meshPath)
{
    if (this->meshPath != meshPath)
    {
        this->meshPath = meshPath;
        isDirty = true;
    }
}

const std::vector<Mrk::MaterialSlot>& Mrk::MeshRenderer::GetMatSlots()
{
    return matSlots;
}

void Mrk::MeshRenderer::SetMatSlots(const std::vector<MaterialSlot>& matSlots)
{
    this->matSlots = matSlots;
}

void Mrk::MeshRenderer::PreDraw()
{
    if (isDirty)
    {
        if (mesh = MeshHut::GetMesh(meshPath))
        {
            auto& subMeshNames = mesh->GetSubMeshNames();

            auto subMeshCount = subMeshNames.size();
            for (size_t i = 0; i < subMeshCount; i++)
            {
                if (matSlots.size() > i)
                {
                    matSlots[i].name = subMeshNames[i];
                }
                else
                {
                    matSlots.emplace_back().name = subMeshNames[i];
                }
            }
            
            for (auto& slot : matSlots)
            {
                slot.Load();
            }

            isDirty = false;
        }
        else
        {
            // mesh error
        }
    }

    if (mesh && !holder.expired())
    {
        RenderItem renderItem;
        renderItem.id = holder.lock()->GetID();
        renderItem.mesh = mesh;
        renderItem.world = Matrix4(1);

        // fix
        {
            std::transform(matSlots.begin(), matSlots.end(), std::back_inserter(renderItem.materials), [](const MaterialSlot& slot) {
                return slot.GetMaterial();
                });
        }


        RenderSys::Commit(RenderLayer::Geometry, renderItem);
    }
}

Mrk::MaterialSlot::MaterialSlot()
{
    static std::string defaultSpName = Mrk::ConfigSys::GetConfigItem<std::string>("ShaderSetting", "defaultShaderProgramName");
    static std::string defaultMatName = Mrk::ConfigSys::GetConfigItem<std::string>("ShaderSetting", "defaultMaterialName");

    spPath = defaultSpName;
    matPath = matPath = defaultMatName;
}

const std::string& Mrk::MaterialSlot::GetName() const
{
    return name;
}

const std::string& Mrk::MaterialSlot::GetSpPath() const
{
    return spPath;
}

void Mrk::MaterialSlot::SetSpPath(const std::string& spPath)
{
    this->spPath = spPath;
}

const std::string& Mrk::MaterialSlot::GetMatPath() const
{
    return matPath;
}

void Mrk::MaterialSlot::SetMatPath(const std::string& matPath)
{
    this->matPath = matPath;
}

std::shared_ptr<Mrk::Material> Mrk::MaterialSlot::GetMaterial() const
{
    return material;
}

void Mrk::MaterialSlot::Load()
{
    if (auto sp = ShaderProgramHut::GetShaderProgram(spPath))
    {
        if (material = MaterialHut::GetMaterial(matPath))
        {
            material->shaderProgram = sp;
        }
    }
}