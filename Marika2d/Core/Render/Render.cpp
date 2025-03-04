#include "Render.h"

void Mrk::RenderSys::Draw()
{
	auto instance = Instance();

	//clear
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	GLuint clearColor[] = { 0, 0, 0, 0 };
	glClearBufferuiv(GL_COLOR, 2, clearColor);
	glClearBufferuiv(GL_COLOR, 1, clearColor);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//background
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	DrawLayer(instance.renderables[(int)RenderLayer::BackGround]);

	//geometry
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	DrawLayer(instance.renderables[(int)RenderLayer::Geometry]);

	//transparent
	//......

	//foreground
	//......
}

void Mrk::RenderSys::Commit(RenderLayer renderLayer, const ObjectRenderParam& renderParam)
{
	auto& instance = Instance();

	assert((int)renderLayer < instance.renderables.size());

	instance.renderables[(int)renderLayer].push_back(renderParam);
}

void Mrk::RenderSys::DrawLayer(const std::vector<ObjectRenderParam>& renderParams)
{

}