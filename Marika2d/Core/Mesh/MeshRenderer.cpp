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
    this->meshPath = meshPath;
    mesh = MeshHut::GetMesh(meshPath);

    auto& subMeshes = mesh->GetSubMeshes();

    if (subMeshes.size() > matSlots.size())
    {
        auto dif = subMeshes.size() - matSlots.size();
        for (size_t i = 0; i < dif; i++)
        {
            if (matSlots.empty())
            {
                auto slot = Mrk::MaterialSlot();
                slot.Load();
                matSlots.push_back(slot);
            }
            else
            {
                matSlots.push_back(matSlots.back());
            }
        }
    }
    else if (subMeshes.size() < matSlots.size())
    {
        matSlots.resize(subMeshes.size());
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

void Mrk::MeshRenderer::Start()
{
    for (auto& slot : matSlots)
    {
        slot.Load();
    }
}

void Mrk::MeshRenderer::PreDraw()
{
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

Mrk::MaterialSlot::MaterialSlot() :
    isShared(true)
{
    static std::string defaultSpName = Mrk::ConfigSys::GetConfigItem<std::string>("ShaderSetting", "defaultShaderProgramName");
    static std::string defaultMatName = Mrk::ConfigSys::GetConfigItem<std::string>("ShaderSetting", "defaultMaterialName");

    spPath = defaultSpName;
    matName = matName = defaultMatName;
}

bool Mrk::MaterialSlot::GetIsShared() const
{
    return isShared;
}

void Mrk::MaterialSlot::SetIsShared(bool val)
{
    isShared = val;
}

const std::string& Mrk::MaterialSlot::GetSpName() const
{
    return spPath;
}

void Mrk::MaterialSlot::SetSpName(const std::string& spPath)
{
    this->spPath = spPath;
}

const std::string& Mrk::MaterialSlot::GetMatName() const
{
    return matName;
}

void Mrk::MaterialSlot::SetMatName(const std::string& matName)
{
    this->matName = matName;
}

std::shared_ptr<Mrk::Material> Mrk::MaterialSlot::GetMaterial() const
{
    return material;
}

void Mrk::MaterialSlot::Load()
{
    if (auto sp = ShaderProgramHut::GetShaderProgram(spPath))
    {
        material = isShared ? sp->GetSharedMaterial(matName) : sp->GetUniqueMaterial();
    }
}