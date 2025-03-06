#include "MeshRenderer.h"

#include "Core/Render/Render.h"
#include "Core/GameObject/GameObject.h"

const std::string& Mrk::MeshRenderer::GetMeshPath()
{
    return meshPath;
}

const std::string& Mrk::MeshRenderer::GetFsPath()
{
    return fsPath;
}

const std::string& Mrk::MeshRenderer::GetVsPath()
{
    return vsPath;
}

void Mrk::MeshRenderer::SetMeshPath(const std::string& meshPath)
{
    this->meshPath = meshPath;
    mesh = MeshHut::GetMesh(meshPath);
}

void Mrk::MeshRenderer::SetFsPath(const std::string& fs)
{
    this->fsPath = fs;
}

void Mrk::MeshRenderer::SetVsPath(const std::string& vs)
{
    this->vsPath = vs;
}

void Mrk::MeshRenderer::Start()
{

}

void Mrk::MeshRenderer::PreDraw()
{
    if (!sp)
    {
        InitSp();
    }

    if (mesh && !holder.expired())
    {
        RenderItem renderItem;
        renderItem.sp = sp;
        renderItem.id = holder.lock()->GetID();
        renderItem.mesh = mesh;
        renderItem.world = Matrix4(1);

        RenderSys::Commit(RenderLayer::Geometry, renderItem);
    }
}

void Mrk::MeshRenderer::InitSp()
{
    sp = ShaderProgramHut::GetShaderProgramID(vsPath, fsPath);

    if (!sp)
    {
        std::cout << "error shader program" << std::endl;
    }
}