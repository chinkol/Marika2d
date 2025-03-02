#include "Render.h"

void Mrk::RenderSys::Draw()
{

}

void Mrk::RenderSys::Commit(RenderLayer renderLayer, const ObjectRenderParam& renderParam)
{
	auto& instance = Instance();

	assert((int)renderLayer < instance.renderables.size());

	instance.renderables[(int)renderLayer].push_back(renderParam);
}