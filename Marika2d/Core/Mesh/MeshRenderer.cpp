#include "MeshRenderer.h"

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
    InitSp();
}

void Mrk::MeshRenderer::InitSp()
{
    sp = ShaderProgramHut::GetShaderProgramID(vsPath, fsPath);
}
