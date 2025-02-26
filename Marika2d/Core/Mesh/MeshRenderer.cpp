#include "MeshRenderer.h"

const std::string& Mrk::MeshRenderer::GetFsPath()
{
    return fs;
}

const std::string& Mrk::MeshRenderer::GetVsPath()
{
    return vs;
}

void Mrk::MeshRenderer::SetFsPath(const std::string& fs)
{
    this->fs = fs;
}

void Mrk::MeshRenderer::SetVsPath(const std::string& vs)
{
    this->vs = vs;
}

void Mrk::MeshRenderer::Start()
{
    InitSp();
}

void Mrk::MeshRenderer::InitSp()
{
    sp = ShaderProgramHut::GetShaderProgramID(vs, fs);
}