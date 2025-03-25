#include "MeshRenderer.h"

#include "Core/GameObject/GameObject.h"
#include "Core/Mesh/Mesh.h"
#include "Core/Render/Render.h"

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

void Mrk::MeshRenderer::Start()
{
    renderItem = std::make_shared<RenderItem>();
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

            isDirty = false;
        }
        else
        {
            // mesh error
        }
    }

    if (mesh && !holder.expired())
    {
        int subMeshIndex = 0;
        for (auto& slot : matSlots)
        {
            auto& subMesh = mesh->GetSubMeshes()[subMeshIndex++];

            renderItem->count = subMesh.count;
            renderItem->offset = subMesh.offset;
            renderItem->mat = slot.material;
            renderItem->id = holder.lock()->GetID();
            renderItem->mesh = mesh;
            renderItem->world = Matrix4(1);

            RenderSys::Commit(renderItem);
        }
    }
}

Mrk::MaterialSlot::MaterialSlot()
{
}

const std::string& Mrk::MaterialSlot::GetName() const
{
    return name;
}

const std::string& Mrk::MaterialSlot::GetMatPath() const
{
    return matPath;
}

void Mrk::MaterialSlot::SetMatPath(const std::string& matPath)
{
    this->matPath = matPath;
    material = MaterialHut::GetMaterial(matPath);
}

std::shared_ptr<Mrk::Material> Mrk::MaterialSlot::GetMaterial() const
{
    return material;
}